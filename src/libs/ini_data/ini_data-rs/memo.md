OpenIniFile:
1. Check if file is already open
2. If yes - return existing FD handle
3. If no - find first unused position in an array of FD handles
4. Read file data
5. Replace all line feeds with '\0'
6. Parse data