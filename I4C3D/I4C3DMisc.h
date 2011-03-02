#pragma once
class I4C3DMisc
{
public:
	I4C3DMisc(void);
	~I4C3DMisc(void);

	BOOL ExecuteOnce(void);
	void Cleanup(void);

	static void GetModuleFileWithExtension(LPTSTR lpszFilePath, SIZE_T size, LPCTSTR lpszExtension);
	static void ReportError(LPCTSTR lpszMessage);
	static void LogDebugMessage(LPCTSTR lpszMessage);
	static void LogDebugMessageA(LPCSTR lpszMessage);
};

