/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/
#include <iostream>
#include "stdafx.h"
#include "sapi.h"
#include "sphelper.h"
#include "cqp.h"
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����

using namespace std;

int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;


LPWSTR StringToWString(const std::string& s)
{
	int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
	return lpwszStr;
}

std::string msgtospeech(int64_t *fromGroup, int64_t *fromQQ, const char *msg) {
	const std::string& fromQQ1 = to_string(static_cast<int>(static_cast<double>(*fromQQ)));
	const std::string& fromGroup1 = to_string(static_cast<int>(static_cast<double>(*fromGroup)));
	const std::string& msg1 = msg;

	if (static_cast<int>(static_cast<double>(*fromGroup)) == 0) {
		return "���ԣ�" + fromQQ1 + "�ģ�" + msg1;
	}
	else
	{
		return "���ԣ�" + fromGroup1 + "�ģ�" +  fromQQ1 + "�ģ�" + msg1;
	}
}


void synthesis(const std::string& s)
{
	::CoInitialize(NULL);             // COM��ʼ��
	CLSID CLSID_SpVoice;
	CLSIDFromProgID(_T("SAPI.SpVoice"), &CLSID_SpVoice);
	ISpVoice* pSpVoice = NULL;
	IEnumSpObjectTokens* pSpEnumTokens = NULL;
	CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)& pSpVoice);
	SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pSpEnumTokens);
	ULONG ulTokensNumber = 0;
	pSpEnumTokens->GetCount(&ulTokensNumber);
	ISpObjectToken* pSpToken = NULL;
	pSpEnumTokens->Item(2, &pSpToken);//����������֪������������Ŷ���
	pSpVoice->SetVoice(pSpToken);

	pSpVoice->Speak(StringToWString(s), SPF_DEFAULT, NULL);     // �ʶ����ĺ�Ӣ�ĵĻ���ַ���
	pSpVoice->Release();
	pSpEnumTokens->Release();
	::CoUninitialize();
}

/* 
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {
	int64_t fromGroup = 0;
	synthesis(msgtospeech(&fromGroup, &fromQQ, msg));
	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ
	return EVENT_BLOCK;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	synthesis(msgtospeech(&fromGroup, &fromQQ, msg));
	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}
