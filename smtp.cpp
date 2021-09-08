#include "smtp.h"
#include <sstream>

smtp::smtp() {
	WSADATA wsaData;
	WORD wVersionRequest = MAKEWORD(2, 1);
	WSAStartup(wVersionRequest, &wsaData);
	client = socket(AF_INET, SOCK_STREAM, 0);
}

void smtp::connectToSever(string severName)
{
	HOSTENT* pHostent;//hostent是host entry的缩写，该结构记录主机的信息，包括主机名、别名、地址类型、地址长度和地址列表
	pHostent = gethostbyname(severName.c_str());  //链接到服务器

	SOCKADDR_IN addSever;
	addSever.sin_addr.S_un.S_addr = *((DWORD*)pHostent->h_addr_list[0]); //得到smtp服务器的网络字节序的ip地址
	addSever.sin_family = AF_INET;
	addSever.sin_port = htons(port);  //链接端口
	connect(client, (SOCKADDR*)&addSever, sizeof(SOCKADDR));
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << buff << endl;

	string msg = "ehlo smtp.163.com\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;
}


void smtp::authLogin(string user, string passWord)
{
	string msg = "auth login\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;

	msg = toBase64(user) + "\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;

	msg = toBase64(passWord) + "\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;
}

void smtp::sendMailFromTo(string from, vector<string>to)
{
	this->from = from;
	this->to = to;
	string msg = "mail from: <" + from+">\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;

	for (int i = 0; i < to.size(); ++i) {
		msg = "rcpt to: <" + to[i] + ">\r\n";
		send(client, msg.c_str(), msg.length(), NULL);
		buff[recv(client, buff, 500, 0)] = '\0';
		cout << "C:" << msg << endl;
		cout << "S:" << buff << endl;
	}

	msg = "data\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;
}

string smtp::readFileAsByte(string fileName)
{
	ifstream readFile(fileName, ios_base::binary);
	ostringstream buffer;
	char ch;
	while (buffer&& readFile.get(ch))
	{
		buffer.put(ch);
	}
	return buffer.str();
}

string smtp::sendTextContents(int Type, string content)
{
	string Content_Type;
	switch (Type)
	{
	case 0:
		Content_Type = "text/plain";
		break;
	case 1:
		Content_Type = "text/html";
		break;
	default:
		break;
	}
	sendContent += "Content-Type: " + Content_Type + "; charset=\"gb2312\"\r\n";
	sendContent += "Content-Transfer-Encoding: base64\r\n";
	sendContent += "Subject: =?gb2312?B?"+toBase64(subject) + "?=\r\n";
	sendContent += "To: ";
	for (int i = 0; i < to.size(); ++i) {
		if (i == to.size() - 1) { sendContent += to[i] + "\r\n"; break; }
		sendContent += to[i] + '\n';
	}
	sendContent += "From: " + from + "\r\n";
	sendContent += "\r\n";
	sendContent += toBase64(content)+"\r\n";
	sendContent += "\r\n.\r\n";
	send(client, sendContent.c_str(), sendContent.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << sendContent << endl;
	cout << "S:" << buff << endl;
	return sendContent;
}


void smtp::setMultipartHeaders(string contentType)  
{
	sendContent += "Content-Type:"+ contentType+"; boundary = \"" + boundary[contentType] + "\"\r\n";
	sendContent += "Subject: =?gb2312?B?" + toBase64(subject) + "?=\r\n";
	sendContent += "To: ";
	for (int i = 0; i < to.size(); ++i) {
		if (i == to.size() - 1) { sendContent += to[i] + "\r\n"; break; }
		sendContent += to[i] + '\n';
	}
	sendContent += "From: " + from + "\r\n";
}


void smtp::addAttchment(string attchmentPath)
{
	sendContent += "\r\n";
	isContainInline?sendContent += "--" + boundary["multipart/related"]+"\r\n":
	               	sendContent += "--" + boundary["multipart/alternative"] + "\r\n";
	sendContent += "\r\n";
	sendContent += "--"+boundary["multipart/mixed"] + "\r\n";
	sendContent += "Content-Type: " + getSubTypeAttachment(attchmentPath);
	sendContent += "Content-Transfer-Encoding: base64\r\n";
	vector<string> fileSplit = split(attchmentPath, "\\");
	string fileName = *(fileSplit.end() - 1);
	sendContent += "Content-Disposition: attachment; filename=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	sendContent += "\r\n";
	for (int i = 0; i < attchmentPath.size(); ++i) {
		if (attchmentPath[i] == '\\') attchmentPath[i] = '/';
	}
	sendContent += toBase64(readFileAsByte(attchmentPath)) + "\r\n";
	sendContent += "\r\n";
	isContainAttachment = true;
}

void smtp::addTextPlain(string content)
{
	sendContent += "\r\n";
	sendContent += "--" + boundary["multipart/alternative"] + "\r\n";
	sendContent += "Content-Type: text/plain; charset=\"gb2312\"\r\n";
	sendContent += "Content-Transfer-Encoding: base64\r\n";
	sendContent += "\r\n";
	sendContent += toBase64(content) + "\r\n";
}

void smtp::addTextHtml(string html)
{
	sendContent += "\r\n";
  sendContent += "--" + boundary["multipart/alternative"] + "\r\n";
	sendContent += "Content-Type: text/html; charset=\"gb2312\"\r\n";
	sendContent += "Content-Transfer-Encoding: base64\r\n";
	sendContent += "\r\n";
	sendContent += toBase64(html) + "\r\n";
}

void smtp::addInline(string filePath)
{
	if (!isContainInline)
	{
		sendContent += "\r\n";
		sendContent += "--" + boundary["multipart/alternative"] + "\r\n";
	}
	sendContent += "\r\n";
	sendContent+= "--" + boundary["multipart/related"] + "\r\n";
	sendContent += "Content-Type: " + getSubTypeAttachment(filePath);
	sendContent += "Content-Transfer-Encoding: base64\r\n";
	isContainInline = true;
}

void smtp::addMultipartContentType(string contentType)
{
	if (contentType == "multipart/related") {
		sendContent += "\r\n";
		sendContent += "--" + boundary["multipart/mixed"] + "\r\n";
		sendContent += "Content-Type:" + contentType + "; boundary = \"" + boundary[contentType] + "\"\r\n";
	}
	else if (contentType == "multipart/alternative") {
		sendContent += "\r\n";
		if(isContainInline)
		sendContent += "--" + boundary["multipart/related"] + "\r\n";
		else sendContent += "--"+boundary["multipart/mixed"] + "\r\n";
		sendContent += "Content-Type:" + contentType + "; boundary = \"" + boundary[contentType] + "\"\r\n";
	}
}

string smtp::getSubTypeAttachment(string fileName)
{
	string res;
	vector<string> fileSplit = split(fileName, "\\");
	fileName = *(fileSplit.end() - 1);
	string fileType = *(split(fileName, ".").end() - 1);
	string ContentType;
	if (fileType == "jpg")ContentType = "image/jpeg; name=\"=?gb2312?B?"+toBase64(fileName) + "?=\"\r\n";
	else if(fileType=="png")ContentType = "image/png; name=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	else if (fileType == "gif")ContentType = "image/gif; name=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	else if (fileType == "txt")ContentType = "text/plain; name=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	else if(fileType=="pdf")ContentType = "application/pdf; name=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	else if(fileType== "doc"||fileType=="docx")
		ContentType = "application/vnd.openxmlformats-officedocument.wordprocessingml.document; name=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	else if(fileType=="xls"||fileType=="xlsx")
		ContentType = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet; name=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	else if(fileType=="ppt")
		ContentType = "application/vnd.openxmlformats-officedocument.presentationml.presentation; name=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	else if(fileType=="zip")ContentType = "application/zip; name=\"=?gb2312?B?" + toBase64(fileName) + "?=\"\r\n";
	return ContentType;
}

void smtp::sendMultipart()
{
	sendContent += "\r\n";
	if(isContainAttachment)
	 sendContent += "--" + boundary["multipart/mixed"] + "\r\n";
	else if(isContainInline)
		sendContent+= "--" + boundary["multipart/related"] + "\r\n";
	else sendContent += "--" + boundary["multipart/alternative"] +"\r\n";
	sendContent += "\r\n.\r\n";
	send(client, sendContent.c_str(), sendContent.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << sendContent << endl;
	cout << "S:" << buff << endl;
}

void smtp::vrfy(string name)
{
	string msg = "vrfy "+name+"\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;
}

void smtp::rset()
{
	string msg = "rset\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;
}

void smtp::quit()
{
	string msg = "quit\r\n";
	send(client, msg.c_str(), msg.length(), NULL);
	buff[recv(client, buff, 500, 0)] = '\0';
	cout << "C:" << msg << endl;
	cout << "S:" << buff << endl;
	closesocket(client);
}

std::vector<std::string> smtp::split(std::string str, std::string delim)
{
	str = str + delim;
	vector<std::string> res;
	for (int i = 0; i < (int)(str.length()); ++i) {
		int pos = static_cast<int>(str.find(delim, i));
		if (pos != str.npos) {
			std::string sub = str.substr(i, pos - i);
			res.push_back(sub);
			i = pos + (int)(delim.length()) - 1;
		}
	}
	return res;
}

std::string smtp::toBase64(string str)
{
	string base64_basic = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	string res;
	string strTobitRes;
	for (int i = 0; i < str.length(); ++i) {
		bitset<8> bit(str[i]);
		strTobitRes += bit.to_string();
	}
	while (true) {                          //不是6的倍数补零
		if (strTobitRes.length() % 6 != 0) {
			strTobitRes = strTobitRes + '0';
		}
		else break;
	}
	for (int i = 0; i < strTobitRes.length(); i = i + 6) {
		string split = strTobitRes.substr(i, 6);
		bitset<10> bit(split);
		res += base64_basic[bit.to_ulong()];
		if (res.length() % 75 == 0) res += '\n';   //每行最多76个字符
	}
	while (true) {
		if (res.length() % 4 != 0) {             //不是4的倍数布‘=’
			res += '=';
		}
		else break;
	}
	return res;
}

std::string smtp::toQuotedPrintable(string str)
{
	string hexChar = "0123456789ABCDEF";
	string res;
	for (int i = 0; i < str.length(); ++i) {
		if ((int)str[i] == 9 && res.length() % 72 == 0) res += "=09=";
		else if ((int)str[i] == 32 && res.length() % 72 == 0) res += "=20=";
		else if (res.length() % 75 == 0 && !res.empty()) res += "=";
		else if (str[i] == '=') res += "=3D";
		else if ((int)str[i] >= 33 && (int)str[i] <= 126) res += str[i];
		else if ((int)str[i] < 33 || (int)str[i]>126) {
			res += "=";
			bitset<8> bit(str[i]);
			string temp = bit.to_string();
			for (int j = 0; j < temp.length(); j = j + 4) {
				string everyCharBinary = temp.substr(j, 4);
				bitset<4> charBit(everyCharBinary);
				res += hexChar[charBit.to_ulong()];
			}
		}
	}
	return res;
}


