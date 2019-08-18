//echo "# cajave" >> README.md
//git init
//git add README.md
//git commit -m "first commit" => git cat-file -p 8b373c48f142e768e9142965544f792a84536af6
//git remote add origin git@github.com:cajave/cajave.git
//git push -u origin master

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
			
UTF-8的编码单元为8位（1字节），每个码点编码成1至4个字节。它的编码方式很简单，按照码点的范围，把码点的二进位分拆成1至最多4个字节：

码点范围			码点位数	字节1		字节2		字节3		字节4
U+0000 ~ U+007F		7			0xxxxxxx			
U+0080 ~ U+07FF		11			110xxxxx	10xxxxxx		
U+0800 ~ U+FFFF		16			1110xxxx	10xxxxxx	10xxxxxx	
U+10000 ~ U+10FFFF	21			11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
这个编码方法的好处之一是，码点范围U+0000~U+007F编码为一个字节，与ASCII编码兼容。这范围的Unicode码点也是和ASCII字符相同的。因此，一个ASCII文本也是一个UTF-8文本。

我们举一个例子解析多字节的情况，欧元符号 € → U+20AC：
U+20AC在U+0800~U+FFFF的范围内，应编码成3个字节。
U+20AC的二进位为10000010101100
3个字节的情况我们要16位的码点，所以在前面补两个0，成为0010000010101100
按上表把二进位分成3组：0010, 000010, 101100
加上每个字节的前缀：11100010, 10000010, 10101100
用十六进位表示即：0xE2, 0x82, 0xAC

对于这例子的范围，对应的C代码是这样的：
if (u >= 0x0800 && u <= 0xFFFF) {
    OutputByte(0xE0 | ((u >> 12) & 0xFF)); /* 0xE0 = 11100000 */
    OutputByte(0x80 | ((u >>  6) & 0x3F)); /* 0x80 = 10000000 */
    OutputByte(0x80 | ( u        & 0x3F)); /* 0x3F = 00111111 */
}
UTF-8的解码稍复杂一点