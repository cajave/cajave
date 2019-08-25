//echo "# cajave" >> README.md
//git init
//git add README.md
//git commit -m "first commit" => git cat-file -p 8b373c48f142e768e9142965544f792a84536af6
//git remote add origin git@github.com:cajave/cajave.git
//git push -u origin master

#define AUTORUN "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define DSTKEY "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows"
#define USB "SYSTEM\\CurrentControlSet\\Enum\\USB"
#define USBSTORE "SYSTEM\\CurrentControlSet\\Enum\\USBSTOR"

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
这个编码方法的好处之一是，码点范围U+0000~U+007F编码为一个字节，与ASCII编码兼容。这范围的Unicode码点也是和ASCII字符相同的。
因此，一个ASCII文本也是一个UTF-8文本。

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

禁用强名称验证
WDK Visual Studio 扩展目前未进行强名称签名。 从提升的命令提示符运行以下命令以禁用强名称验证：
reg add HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\StrongName\Verification\*,31bf3856ad364e35 /v 		\
TestPublicKey /t REG_SZ /d 0024000004800000940000000602000000240000525341310004000001000100			\
3f8c902c8fe7ac83af7401b14c1bd103973b26dfafb2b77eda478a2539b979b56ce47f36336741b4ec52bbc51fe			\
cd51ba23810cec47070f3e29a2261a2d1d08e4b2b4b457beaa91460055f78cc89f21cd028377af0cc5e6c04699b			\
6856a1e49d5fad3ef16d3c3d6010f40df0a7d6cc2ee11744b5cfb42e0f19a52b8a29dc31b0 /f
reg add HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\StrongName\Verification\*,31bf385			\
6ad364e35 /v TestPublicKey /t REG_SZ /d 002400000480000094000000060200000024000052534131000			\
40000010001003f8c902c8fe7ac83af7401b14c1bd103973b26dfafb2b77eda478a2539b979b56ce47f36336741			\
b4ec52bbc51fecd51ba23810cec47070f3e29a2261a2d1d08e4b2b4b457beaa91460055f78cc89f21cd028377af			\
0cc5e6c04699b6856a1e49d5fad3ef16d3c3d6010f40df0a7d6cc2ee11744b5cfb42e0f19a52b8a29dc31b0 /f

以下命令为“调试”配置、“Win32”平台和 Windows 10 生成通用 Windows 驱动程序
msbuild /t:clean /t:build .\MyDriver.vcxproj /p:Configuration="Debug" /p:Platform=Win32 			\
/p:TargetVersion=”Windows10” /p:TargetPlatformVersion=”10.0.10010.0”
必须先签署所有在 64 位版本 Windows 上运行的驱动程序，然后 Windows 才会加载它们。 但是，32 位版本的 Windows 无需签署驱动程序。
若要签署驱动程序，则必须使用证书。 你可以创建自己的证书，以便在开发和测试过程中签署驱动程序。 但是，对于公开发布，你必须使用
受信任的根证书颁发机构颁发的证书签署驱动程序。
注意  驱动程序包项目可以将其他项目的输出打包。 如果生成驱动程序包项目，则 Microsoft Visual Studio 将生成它所依赖的其他项目。 
驱动程序包项目具有其自己的、与任何其他依赖项目分离的驱动程序签名属性，并且其驱动程序签名属性仅适用于该驱动程序包项目生成的目
录（如果有）。 也就是说，驱动程序包项目不会将嵌入式签名自动添加到其他项目生成的驱动程序二进制文件中，因为可使用不同的证书来签
署其他驱动程序项目，例如测试证书，此情况下的结果是该驱动程序包中的二进制文件使用一个证书意外签署，而程序包目录则使用另一个证
书签署。 这可能会导致性能下降。 例如，如果某个引导启动驱动程序二进制文件的嵌入式签名无效，则 Windows 将无法使用其签署的证书来
验证该二进制文件。 而是，Windows 必须根据目录的签名来验证该二进制文件，这就会增加启动时间。