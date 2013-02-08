/*
Winprint HylaFAX Reloaded
Copyright (C) 2011-2013 Monti Lorenzo

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "ipc.h"
#include <windows.h>
#include <wtsapi32.h>

#pragma link "wtsapi32.lib"

#define BUFSIZE 4096

static LPCWSTR szPipeTemplate = L"\\\\.\\pipe\\wphf_sessid%0.8X";
static HANDLE hThread = NULL;
static HANDLE hStop = NULL;
static HANDLE hPipe = NULL;
static OVERLAPPED ov;
static char* buf = NULL;
static DWORD bufSize = 0;
static BOOL bStarted = FALSE;

typedef struct {
	IPCCALLBACK callback;
	LPVOID param;
} THREADDATA, *LPTHREADDATA;

/*
  Alloca memoria per il buffer di ricezione
*/
static BOOL ReAllocBuf(DWORD dwBytes)
{
	HANDLE hHeap = GetProcessHeap();

	//libera la memoria se era gi� allocata
	if (buf)
	{
		HeapFree(hHeap, 0, buf);
		buf = NULL;
		bufSize = 0;
	}

	//se dwBytes == 0 abbiamo finito
	if (dwBytes == 0)
		return TRUE;

	//tenta di allocare la memoria
	if ((buf = HeapAlloc(hHeap, 0, dwBytes)) == NULL)
		return FALSE;

	//legge le dimensioni effettive del blocco allocato
	bufSize = (DWORD)HeapSize(hHeap, 0, buf);

	return TRUE;
}

/*
  Tenta di leggere un certo quantitativo di bytes entro un certo timeout
*/
static BOOL ReadFromPipe(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, DWORD dwMilliseconds)
{
	DWORD nLeft = nNumberOfBytesToRead;
	DWORD dwMsLeft = 0, dwStart = 0;
	DWORD cbRead = 0;
	LPVOID lpCurrent = lpBuffer;
	HANDLE hEvents[2];
	BOOL bWaitingRead = FALSE;

	hEvents[0] = hStop;		// evento 0 = segnale di uscita
	hEvents[1] = ov.hEvent;	// evento 1 = operazione overlapped completata

	//momento di inizio
	dwStart = GetTickCount();

	//cicla finch� non sono arrivati tutti i byte che aspettavamo
	while (nLeft > 0)
	{
		//se non c'� una read in attesa di completamento,
		//ne iniziamo una nuova
		if (!bWaitingRead)
		{
			if (!ReadFile(hPipe, lpCurrent, nLeft, NULL, &ov))
			{
				if (GetLastError() != ERROR_IO_PENDING)
					return FALSE;
				bWaitingRead = TRUE;
			}
		}

		//se l'operazione � in stato di ERROR_IO_PENDING...
		if (bWaitingRead)
		{
			//quanti millisecondi abbiamo ancora?
			dwMsLeft = (dwMilliseconds == INFINITE)
				? INFINITE
				: dwStart + dwMilliseconds - GetTickCount();

			//test se tempo scaduto
			if (dwMsLeft != INFINITE && dwMsLeft <= 0)
				return FALSE;
			
			//restiamo in attesa di un evento
			switch (WaitForMultipleObjects(2, hEvents, FALSE, dwMsLeft))
			{
			case WAIT_OBJECT_0 + 0:	// uscita immediata
				return FALSE;
				break;
			case WAIT_OBJECT_0 + 1:	// operazione completata
				break;
			case WAIT_TIMEOUT:		// timeout, facciamo un altro giro
				continue;
				break;
			default:				// non dovremmo mai arrivare qui
				return FALSE;
				break;
			}
		}

		//controlliamo l'esito dell'operazione asincrona
		if (!GetOverlappedResult(hPipe, &ov, &cbRead, FALSE))
			return FALSE;

		//decremento numero di bytes rimasti e avanzo il puntatore al buffer
		nLeft -= cbRead;
		lpCurrent = (char*)lpCurrent + cbRead;
	}

	return TRUE;
}

/*
  Una volta connessa la pipe, legge i dati in ingresso
  per prima cosa leggiamo 4 byte (lunghezza della stringa che seguir�)
  poi leggiamo la stringa che rappresenta il nome del file
*/
void IpcChat(IPCCALLBACK pfnCallback, LPVOID param)
{
	DWORD len = 0;

	//leggiamo len (4 byte) entro 1 secondo
	if (!ReadFromPipe(&len, sizeof(len), 1000))
		return;

	//perch� dovrebbe succedere? mah...
	if (len == 0)
		return;

	//se serve pi� spazio, allarghiamo il buffer
	//vengono allocati blocchi multipli di 1024 byte
	if ((!buf || len > bufSize) &&
		!ReAllocBuf(((len & -1024) | 1023) + 1))
		return;

	//infine, leggiamo il nome del file
	if (!ReadFromPipe(buf, len, 1000))
		return;

	//terminiamo la stringa
	buf[len] = '\0';

	//e chiamiamo la callback
	pfnCallback(buf, param);
}

/*
  Entry point del thread di gestione della pipe
*/
DWORD WINAPI IpcRoutine(LPVOID lpvParam)
{
	DWORD dwRet = 0, dwLe = 0, cbRet = 0;
	BOOL bDone = FALSE;
	HANDLE hEvents[2];
	LPTHREADDATA ptd = (LPTHREADDATA)lpvParam;
	WCHAR szPipeName[32];
	LPWSTR pBuffer = NULL;
	DWORD cbReturned;
	DWORD dwSessionId = 0;

	if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION,
	WTSSessionId, &pBuffer, &cbReturned)) {
		dwSessionId = *(DWORD*)pBuffer;
		WTSFreeMemory(pBuffer);
	} else {
		if ((dwRet = GetLastError()) != ERROR_APP_WRONG_OS)
			goto cleanup;
	}

	wsprintfW(szPipeName, szPipeTemplate, dwSessionId);

	//creiamo un evento a reset manuale per la notifica dell'I/O asincrono
	ZeroMemory(&ov, sizeof(ov));
	ov.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

	if (ov.hEvent == NULL)
	{
		dwRet = GetLastError();
		goto cleanup;
	}

	hEvents[0] = hStop;		// evento 0 = segnale di uscita
	hEvents[1] = ov.hEvent;	// evento 1 = operazione overlapped completata

	//creiamo la pipe
	hPipe = CreateNamedPipeW(
		szPipeName,
		PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
		PIPE_WAIT | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		1,
		BUFSIZE,
		BUFSIZE,
		0,
		NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		dwRet = GetLastError();
		goto cleanup;
	}

	//ciclo finch� non ricevo il segnale di stop
	while (!bDone)
	{
		//con FILE_FLAG_OVERLAPPED, il ritorno deve essere 0 altrimenti significa errore
		if (ConnectNamedPipe(hPipe, &ov) != 0)
		{
			dwRet = GetLastError();
			goto cleanup;
		}

		//vediamo cosa sta succedendo
		switch (dwLe = GetLastError())
		{
		case ERROR_PIPE_CONNECTED:	// il client si � connesso tra CreateNamedPipeA e ConnectNamedPipe: connessione buona
			break;
		case ERROR_IO_PENDING:		// l'operazione � in attesa di completamento
			switch (WaitForMultipleObjects(2, hEvents, FALSE, INFINITE))
			{
			case WAIT_OBJECT_0 + 0:	// uscita immediata
				bDone = TRUE;
				continue;
				break;
			case WAIT_OBJECT_0 + 1:	// segnale di operazione completata
				//la connessione � andata bene?
				if (!GetOverlappedResult(hPipe, &ov, &cbRet, FALSE))
				{
					dwRet = GetLastError();
					goto cleanup;
				}
				//let's chat!!
				IpcChat(ptd->callback, ptd->param);
				//chiudiamo la pipe
				DisconnectNamedPipe(hPipe);
				break;
			default:				// non dovremmo mai arivare qui
				dwRet = GetLastError();
				goto cleanup;
				break;
			}
			break;
		default:	// si � verificato qualche errore...
			dwRet = dwLe;
			goto cleanup;
		}
	}

cleanup:
	free(lpvParam);
	if (hPipe)
		CloseHandle(hPipe);
	if (ov.hEvent)
		CloseHandle(ov.hEvent);
	return dwRet;
}

/*
  Avvia il therad di gestione della pipe
*/
int StartIpc(IPCCALLBACK pfnCallback, LPVOID param)
{
	DWORD dwThreadId = 0;
	DWORD dwRet = 0;
	LPTHREADDATA ptd;

	//creiamo l'evento di segnalazione di stop
	hStop = CreateEventW(NULL, FALSE, FALSE, NULL);

	if (hStop == NULL)
	{
		dwRet = GetLastError();
		goto cleanup;
	}

	//creiamo il thread, passando il puntatore alla funzione callback
	ptd = malloc(sizeof(THREADDATA));
	ptd->callback = pfnCallback;
	ptd->param = param;
	hThread = CreateThread(
		NULL,
		0,
		IpcRoutine,
		(LPVOID)ptd,
		0,
		&dwThreadId);

	if (hThread == NULL)
	{
		dwRet = GetLastError();
		goto cleanup;
	}

	bStarted = TRUE;

	//tutto ok
	return 0;

cleanup:
	if (hStop)
		CloseHandle(hStop);
	if (hThread)
		CloseHandle(hThread);
	return dwRet;
}

/*
  Ferma il thread di gestione della pipe
*/
void StopIpc()
{
	if (!bStarted)
		return;

	//segnala stop
	SetEvent(hStop);

	//attende chiusura del thread
	WaitForSingleObject(hThread, INFINITE);

	//chiusura handle e memoria allocata
	CloseHandle(hThread);
	CloseHandle(hStop);
	ReAllocBuf(0);

	bStarted = FALSE;
}
