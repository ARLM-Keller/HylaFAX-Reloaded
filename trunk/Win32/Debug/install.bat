NET STOP SPOOLER
COPY /Y D:\proj\wphf-reloaded\Win32\debug\wphfmon.dll C:\Windows\System32
COPY /Y D:\proj\wphf-reloaded\Win32\debug\wphfmonui.dll C:\Windows\System32
NET START SPOOLER
PAUSE