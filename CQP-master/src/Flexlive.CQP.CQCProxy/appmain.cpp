/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "shellapi.h";
#include "string"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
#include "Flexlive.CQP.CQCProxy.h"


using namespace std;
using namespace FlexliveCQP;
using namespace System::Diagnostics;

int ac = -1;
bool enabled = false;

/*
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() 
{
	return CQAPPINFO;
}


/*
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;

	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	FlexliveCQP::CQCPlugin::Initialize(gcnew String(CQ_getAppDirectory(ac)));

	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}


/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {
	
	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	FlexliveCQP::CQCPlugin::CSharpTrans("PrivateMessage", subType, sendTime, 0, 0, fromQQ, "", 0, gcnew String(msg), font, "", "");

	return EVENT_IGNORE;
}


/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	FlexliveCQP::CQCPlugin::CSharpTrans("GroupMessage", subType, sendTime, fromGroup, 0, fromQQ, gcnew String(fromAnonymous), 0, gcnew String(msg), font, "", "");
	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	FlexliveCQP::CQCPlugin::CSharpTrans("DiscussMessage", subType, sendTime, 0, fromDiscuss, fromQQ, "", 0, gcnew String(msg), font, "", "");
	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


///*
//* Type=11 群文件上传
//* subType 子类型，1/被取消管理员 2/被设置管理员
//*/
//CQEVENT(int32_t, __eventGroupUpload, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *file) {
//
//	FlexliveCQP::CQCPlugin::CSharpTrans("GroupUpload", subType, sendTime, fromGroup, 0, fromQQ, "", 0, "", 0, "", gcnew String(file));
//
//	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
//}

/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	FlexliveCQP::CQCPlugin::CSharpTrans("GroupAdmin", subType, sendTime, fromGroup, 0, 0, "", beingOperateQQ, "", 0, "", "");
	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	FlexliveCQP::CQCPlugin::CSharpTrans("GroupMemberDecrease", subType, sendTime, fromGroup, 0, fromQQ, "", beingOperateQQ, "", 0, "", "");
	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	FlexliveCQP::CQCPlugin::CSharpTrans("GroupMemberIncrease", subType, sendTime, fromGroup, 0, fromQQ, "", beingOperateQQ, "", 0, "", "");
	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	FlexliveCQP::CQCPlugin::CSharpTrans("FriendAdded", subType, sendTime, 0, 0, fromQQ, "", 0, "", 0, "", "");
	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");
	FlexliveCQP::CQCPlugin::CSharpTrans("RequestAddFriend", subType, sendTime, 0, 0, fromQQ, "", 0, gcnew String(msg), 0, gcnew String(responseFlag), "");
	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}
	FlexliveCQP::CQCPlugin::CSharpTrans("RequestAddGroup", subType, sendTime, fromGroup, 0, fromQQ, "", 0, gcnew String(msg), 0, gcnew String(responseFlag), "");
	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)() {
	
	ShellExecute(NULL, L"open", L"https://cqp.cc/t/29261", NULL, NULL, SW_HIDE);

	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	//MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "", 0);

	String^ appDirectory = Path::Combine(gcnew String(CQ_getAppDirectory(ac)), "..\\");
	appDirectory = Path::Combine(appDirectory, "..\\");
	appDirectory = Path::Combine(appDirectory, "Flexlive.CQP.CSharpProxy.exe");
	appDirectory = appDirectory + " CLR";

	char* ch2 = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(appDirectory);

	WinExec(ch2, SW_SHOW);

	return 0;
}

__declspec(dllexport) int GetAuthCode()
{
	return ac;
}



