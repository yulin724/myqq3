/*
 *  myqq.c
 *
 *  main() A small console for MyQQ.
 *
 *  Copyright (C) 2008  Huang Guan (gdxxhg@gmail.com)
 *
 *  2008-01-31 Created.
 *  2008-2-5   Add more commands.
 *  2008-7-15  Mr. Wugui said "There's no accident in the world.", 
 *		   but I always see accident in my code :)
 *  2008-10-1  Character color support and nonecho password.
 *  2009-1-25  Use UTF-8 as a default type.
 *
 *  Description: This file mainly includes the functions about 
 *			   Parsing Input Commands.
 *			   
 */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifdef __WIN32__
#include <conio.h>
#include <winsock.h>
#include <wininet.h>
#include <windows.h>
#else
#include <termios.h> //read password
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include "qqclient.h"
#include "buddy.h"
#include "qun.h"
#include "group.h"
#include "memory.h"
#include "utf8.h"
#include "config.h"
#include "qqsocket.h"

#define QUN_BUF_SIZE	80*100
#define BUDDY_BUF_SIZE	80*500
#define PRINT_BUF_SIZE	80*500*3

static char* qun_buf, *buddy_buf, *print_buf;
static uint to_uid = 0;		//send message to that guy.
static uint qun_int_uid;	//internal qun id if entered.
static char input[1024];
static int enter = 0;
static qqclient* qq;
static int need_reset, no_color = 0;
enum{
	CMD_EXIT = 0, CMD_EXIT2,
	CMD_SAY, CMD_SAY2,
	CMD_TO, CMD_TO2,
	CMD_HELP, CMD_HELP2,
	CMD_STATUS,
	CMD_ENTER, CMD_ENTER2,
	CMD_LEAVE, CMD_LEAVE2,
	CMD_WHO, CMD_WHO2,
	CMD_VIEW, CMD_VIEW2,
	CMD_QUN, CMD_QUN2,
	CMD_INFO, CMD_INFO2,
	CMD_UPDATE, CMD_UPDATE2,
	CMD_CHANGE, CMD_CHANGE2,
	CMD_TEST,
	CMD_VERIFY, CMD_VERIFY2,
	CMD_ADD, CMD_ADD2,
	CMD_DEL,
	CMD_CLS, CMD_CLS2, CMD_CLS3
};
static char commands[][16]={
	"exit", "x",
	"say", "s",
	"to", "t",
	"help", "h",
	"status",
	"enter", "e",
	"leave", "l",
	"who", "w",
	"view", "v",
	"qun", "q",
	"info", "i",
	"update", "u",
	"change", "c",
	"test",
	"verify", "r",
	"add", "a",
	"del",
	"cls", "clear", "clrscr"
};

static char help_msg[]=
"欢迎使用 MyQQ2009 中文版\n"
"这是一个为程序员和电脑爱好者制作的迷你控制台即时通讯软件,享受它吧!\n"
"\n"
"help/h:    显示以下帮助信息.\n"
"add/a:     添加好友. add+QQ号码.\n"
"cls/clear: 清屏.\n"
"view/v:    所有好(群)友列表.(指向前操作)\n"
"who/w:     在线好(群)友列表.(指向前操作)\n"
"qun/q:     显示群列表.(指向前操作)\n"
"to/t:      指向某个QQ号或者前面的编号.\n"
"enter/e:   指向某个群号或者前面的编号.\n"
"leave/l:   离开群.(指向后操作)\n"
"say/s:     发送信息.(指向后操作)\n"
"info/i:    查看相应信息.(指向后操作)\n"
"update/u:  更新所有列表.\n"
"status:    改变状态(online|away|busy|killme|hidden)\n"
"verify/r:  输入验证码(验证码在verify目录下).\n"
"change/c:  更换用户登陆.\n"
"exit/x:    退出.\n\n"
;

#ifdef __WIN32__
#define CCOL_GREEN	FOREGROUND_GREEN
#define CCOL_LIGHTBLUE	FOREGROUND_BLUE | FOREGROUND_GREEN
#define CCOL_REDGREEN	FOREGROUND_RED | FOREGROUND_GREEN
#define CCOL_NONE		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define COLOR_TYPE int
static void setcolor( COLOR_TYPE col )
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | col);
}
static void clear_screen()
{
	system("cls");
}
#else

#define CCOL_NONE           "\033[0m"   
#define CCOL_BLACK          "\033[0;30m"   
#define CCOL_DARKGRAY       "\033[1;30m"   
#define CCOL_BLUE           "\033[0;34m"   
#define CCOL_LIGHTBLUE      "\033[1;34m"   
#define CCOL_GREEN          "\033[0;32m"   
#define CCOL_LIGHTGREEN    	"\033[1;32m"   
#define CCOL_CYAN	    	"\033[0;36m"   
#define CCOL_LIGHTCYAN    	"\033[1;36m"   
#define CCOL_RED	    	"\033[0;31m"   
#define CCOL_LIGHTRED	    "\033[1;31m"   
#define CCOL_PURPLE         "\033[0;35m"   
#define CCOL_LIGHTPURPLE    "\033[1;35m"   
#define CCOL_LIGHTBROWN     "\033[0;33m"   
#define CCOL_YELLOW         "\033[1;33m"   
#define CCOL_LIGHTGRAY      "\033[0;37m"   
#define CCOL_WHITE          "\033[1;37m"
#define CCOL_REDGREEN       CCOL_YELLOW
#define COLOR_TYPE const char *
static void setcolor( COLOR_TYPE col )
{
	printf( col );
}
static void clear_screen()
{
	//system("clear");
	printf("\033[2J   \033[0;0f");
}
#endif

static int inputline(char *s, int lim) 
{
	char *t;
	int c;

	t = s;
	while (--lim>1 && (c=getchar()) != EOF && c != '\n')
		*t++ = c;
	*t = '\0';
#ifdef __WIN32__
	gb_to_utf8(s, s, lim);
#endif
	return strlen(s);
}

static char * qqstr2uistr(char * str)
{
#ifdef __WIN32__
	utf8_to_gb( str, print_buf, PRINT_BUF_SIZE-1 );
	return print_buf;
#else
	return str;
#endif
}

static char * qqstr2gbstr(char * str, char * dst, int len)
{
	if(len < 2)
		return str;
	utf8_to_gb( str, dst, len - 1 );
	dst[len - 1] = 0;
	return dst;
}

static void print_string(char *str)
{
#ifdef __WIN32__
	printf( str );
#else
	char tempstr[1024];
	gb_to_utf8( str, tempstr, sizeof(tempstr) );
	printf( tempstr );
	fflush(stdout);
#endif
}

static void puts_color(char * str, COLOR_TYPE color)
{
	if( no_color )
	{
		puts( qqstr2uistr( str ) );
	}
	else
	{
		setcolor( color );
		puts( qqstr2uistr( str ) );
		setcolor( CCOL_NONE );
	}
}

static char* mode_string( int mode )
{
	switch( mode ){
	case QQ_ONLINE:
		return "Online";
	case QQ_AWAY:
		return "Away";
	case QQ_BUSY:
		return "Busy";
	case QQ_OFFLINE:
		return "Offline";
	case QQ_HIDDEN:
		return "Hidden";
	case QQ_KILLME:
		return "Kill Me";
	case QQ_QUIET:
		return "Quiet";
	}
	return "Unknown";
}
static char* skip_line( char* p, int ln )
{
	while( *p && ln-- )
	{
		p ++;
		while( *p && *p!='\n' ) p++;
	}
	return p;
}
static char* myqq_get_buddy_name( qqclient* qq, uint uid )
{
	static char tmp[16];
	qqbuddy* b = buddy_get( qq, uid, 0 );
	if( b )
		return b->nickname;
	if( uid == 10000 )
		return "System";
	if( uid != 0 ){
		sprintf( tmp, "%u" , uid );
		return tmp;
	}
	return "Nobody";
}
static char* myqq_get_qun_name( qqclient* qq, uint uid )
{
	static char tmp[16];
	qqqun* q = qun_get( qq, uid, 1 );
	if( q )
		return q->name;
	if( uid != 0 ){
		sprintf( tmp, "%u" , uid );
		return tmp;
	}
	return "[q==NULL]";
}
static char* myqq_get_qun_member_name( qqclient* qq, uint int_uid, uint uid )
{
	static char tmp[16];
	qqqun* q = qun_get( qq, int_uid, 0 );
	if( q ){
		qunmember* m = qun_member_get( qq, q, uid, 0 );
		if( m )
			return m->nickname;
		if( uid != 0 ){
			sprintf( tmp, "%u" , uid );
			return tmp;
		}
		return "[m==NULL]";
	}
	return "[q==NULL]";
}
static int myqq_send_im_to_qun( qqclient* qq, uint int_uid, char* msg, int wait )
{
	qun_send_message( qq, int_uid, msg );
	if( wait )
	{
		if( qqclient_wait( qq, 15 ) < 0 )
			return -1;
	}
	return 0;
}
static int myqq_send_im_to_buddy( qqclient* qq, uint int_uid, char* msg, int wait )
{
	buddy_send_message( qq, int_uid, msg );
	if( wait )
	{
		if( qqclient_wait( qq, 15 ) < 0 )
			return -1;
	}
	return 0;
}

static int myqq_get_buddy_info( qqclient* qq, uint uid, char* buf, int size )
{
	qqbuddy *b = buddy_get( qq, uid, 0 );
	if( size < 256 )
		return -1;
	if( b == NULL )
		return -2;
	int len;
	char nickname[0x10];
	char signature[0x40];
	char alias[0x40];
	char mobilephone[0x10];
	char email[0x20];
	char occupation[0x10];
	char homepage[0x40];
	char school[0x40];
	char country[0x10];
	char province[0x10];
	char city[0x10];
	char address[0x40];
	char brief[0x100];
	len = sprintf( buf,	
				"好友昵称\t%s\n"
				"用户账号\t%u\n"
				"签名\t\t%s\n"
				"备注\t\t%s\n"
				"手机\t\t%s\n"
				"邮箱\t\t%s\n"
				"职业\t\t%s\n"
				"主页\t\t%s\n"
				"毕业学院\t%s\n"
				"来自\t\t%s %s %s\n"
				"通讯地址\t%s\n"
				"自我介绍\t%s\n"
				"头像\t\t%d\n"
				"年龄\t\t%d\n"
				"性别\t\t%s\n"
				"状态\t\t%s\n",
		qqstr2gbstr(b->nickname, nickname, sizeof(nickname)),
		b->number, 
		qqstr2gbstr(b->signature, signature, sizeof(signature)),
		qqstr2gbstr(b->alias, alias, sizeof(alias)),
		qqstr2gbstr(b->mobilephone, mobilephone, sizeof(mobilephone)),
		qqstr2gbstr(b->email, email, sizeof(email)), 
		qqstr2gbstr(b->occupation, occupation, sizeof(occupation)), 
		qqstr2gbstr(b->homepage, homepage, sizeof(homepage)), 
		qqstr2gbstr(b->school, school, sizeof(school)), 
		qqstr2gbstr(b->country, country, sizeof(country)), 
		qqstr2gbstr(b->province, province, sizeof(province)), 
		qqstr2gbstr(b->city, city, sizeof(city)),
		qqstr2gbstr(b->address, address, sizeof(address)), 
		qqstr2gbstr(b->brief, brief, sizeof(brief)), 
		b->face, 
		b->age,
		(b->sex==0x00)?"Male": (b->sex==0x01)?"Female":"Asexual", 
		mode_string(b->status) );
	return len;
}


//Note: this function change the source string directly.
static char* util_escape( char* str )
{
	unsigned char* ch;
	ch = (unsigned char*)str;
	while( *ch )
	{
		if( *ch < 0x80 ) //ASCII??
		{
			if( !isprint(*ch) ) //if not printable??
				*ch = ' ';	//use space instead..
			ch ++;	//skip one
		}else{
			ch +=2; //skip two
		}
	}
	return str;
}

/*   The output buf looks like this:
L8bit  L16bit		L16bit		L32bit
1	  357339036	 online		Xiao xia
2	  273310179	 offline	Huang Guan
*/
//Note: size must be big enough
static int myqq_get_buddy_list( qqclient* qq, char* buf, int size, char online )
{
	int i, len = 0;
	//avoid editing the array
	buf[0] = 0;
	pthread_mutex_lock( &qq->buddy_list.mutex );
	int ln = 1;
	char alias[0x20];
	char nickname[0x20];
	for( i=0; i<qq->buddy_list.count; i++ )
	{
		qqbuddy * b = (qqbuddy*)qq->buddy_list.items[i];
		if( online && b->status == QQ_OFFLINE ) continue;
		if( *b->alias ){
			len = sprintf( buf, "%s%-8d%-16u%-16s%-16.64s\n", buf, ln ++, b->number, 
				mode_string( (int) b->status ), 
				qqstr2gbstr(util_escape( b->alias ), alias, sizeof(alias)) );
		}else{
			len = sprintf( buf, "%s%-8d%-16u%-16s%-16.64s\n", buf, ln ++, b->number, 
				mode_string( (int) b->status ), 
				qqstr2gbstr(util_escape( b->nickname ), nickname, sizeof(nickname)) );
		}
		if( len + 80 > size ) break;
	}
	pthread_mutex_unlock( &qq->buddy_list.mutex );
	return len;
}

/*   The output buf looks like this:
L8bit  L16bit		L16bit		L32bit
1	  467788923	 12118724	Xinxing Experimental School
2	  489234223	 13223423	SGOS2007
*/
//Note: size must be big enough
static int myqq_get_qun_list( qqclient* qq, char* buf, int size )
{
	int i, len = 0, ln=1;
	//avoid editing the array
	buf[0] = 0;
	char name[0x20];

	pthread_mutex_lock( &qq->qun_list.mutex );
	for( i=0; i<qq->qun_list.count; i++ )
	{
		qqqun * q = (qqqun *)qq->qun_list.items[i];
		len = sprintf( buf, "%s%-8d%-16u%-16u%-16.64s\n", buf, ln ++, q->number, 
			q->ext_number, 
			qqstr2gbstr(util_escape( q->name ), name, sizeof(name)) );
		if( len + 80 > size ) break;
	}
	pthread_mutex_unlock( &qq->qun_list.mutex );
	return len;
}

/*   The output buf looks like this:
L8bit  L16bit		L16bit		L32bit
1	  357339036	 Manager	Xiaoxia
2	  273310179	 Fellow		Huang Guan
*/
//Note: size must be big enough
static int myqq_get_qun_member_list( qqclient* qq, uint int_uid, char* buf, int size, char online )
{
	qqqun * q = qun_get( qq, int_uid, 0 );
	if( !q )return 0;
	//Hope the Qun won't get removed while we are using it. 
	int i, len = 0, ln = 1;
	buf[0] = 0;
	char nickname[0x20];

	pthread_mutex_lock( &q->member_list.mutex );
	for( i=0; i<q->member_list.count; i++ )
	{
		qunmember * m = (qunmember *)q->member_list.items[i];
		if( online && m->status == QQ_OFFLINE ) continue;
		len = sprintf( buf, "%s%-8d%-16u%-16s%-16.64s\n", buf, ln++, m->number, 
			(m->role&1)?"Admin":"Fellow", 
			qqstr2gbstr(util_escape( m->nickname ), nickname, sizeof(nickname)) );
		if( len + 80 > size )
			break;
	}
	pthread_mutex_unlock( &q->member_list.mutex );
	return len;
}

//interface for getting qun information
/* Output style:
*/
static int myqq_get_qun_info( qqclient* qq, uint int_uid, char* buf, int size )
{
	char cate_str[4][10] = {"Classmate", "Friend", "Workmate", "Other" };
	qqqun *q = qun_get( qq, int_uid, 0 );
	if( !q )return 0;
	int len;
	if( size < 256 )
		return -1;

	if( q == NULL )
		return -2;

	char name[0x10];
	char intro[0x100];
	char ann[0x100];

	len = sprintf( buf, 	
				"名称\t\t%s\n"
				"内部号码\t%u\n"
				"群号码\t\t%u\n"
				"群类型\t\t%s\n"
				"加入验证\t%s\n"
				"群分类\t\t%s\n"
				"创建人\t\t%u\n"
				"成员数量\t%d\n"
				"群的简介\n%s\n"
				"群的公告\n%s\n",
		qqstr2gbstr(q->name, name, sizeof(name)),
		q->number, 
		q->ext_number, 
		(q->type==0x01)?"Normal":"Special",
		(q->auth_type==0x01)?"No": (q->auth_type==0x02)?"Yes": (q->auth_type==0x03)?"RejectAll":"Unknown",
		q->category > 3 ? cate_str[3] : cate_str[(int)q->category],
		q->owner, 
		q->member_list.count, 
		qqstr2gbstr(q->intro, intro, sizeof(intro)),  
		qqstr2gbstr(q->ann, ann, sizeof(ann))  
		);
	return len;
}

void reset_input()
{
	if( need_reset )
	{
		char * pstr;
		if( enter ) 
		{
			pstr = myqq_get_qun_name( qq, qun_int_uid );
			printf( "In {%s}> ", qqstr2uistr(pstr) );
		}
		else
		{
			pstr = myqq_get_buddy_name( qq, to_uid );
			printf( "To [%s]> ", qqstr2uistr(pstr) );
		}
		fflush( stdout );
		need_reset = 0;
	}
}

//含颜色控制
void buddy_msg_callback ( qqclient* qq, uint uid, time_t t, char* msg )
{
	char timestr[12];
	char msgstr[64];
	struct tm * timeinfo;
	char* nick = myqq_get_buddy_name( qq, uid );
  	timeinfo = localtime ( &t );
	strftime( timestr, 12, "%H:%M:%S", timeinfo );
	char tmp[20], *p;
	sprintf( tmp, "%-16d", uid );
	p = strstr( buddy_buf, tmp );
	if( p )
	{
		p -= 8;
		if( p>=buddy_buf )
		{
			int ln;
			sscanf( p, "%d", &ln );
			sprintf( msgstr, "\n%d)%s[", ln, timestr );
			strcat( msgstr, nick );
			strcat( msgstr, "]\n\t" );//二次修改 
			print_string( msgstr );
			puts_color( msg, CCOL_LIGHTBLUE );	//red color
			reset_input();
		}
	}else{
		sprintf( msgstr, "\n%s[", timestr );
		strcat( msgstr, nick );
		strcat( msgstr, "]\n\t" );//二次修改 
		print_string( msgstr );
		puts_color( msg, CCOL_LIGHTBLUE );	//red color
		reset_input();
	}
}

//含颜色控制
void qun_msg_callback ( qqclient* qq, uint uid, uint int_uid,
	time_t t, char* msg )
{
	char timestr[12];
	char msgstr[64];
	char* qun_name = myqq_get_qun_name( qq, int_uid );
	char* nick = myqq_get_qun_member_name( qq, int_uid, uid );
	struct tm * timeinfo;
  	timeinfo = localtime ( &t );
	strftime( timestr, 12, "%H:%M:%S", timeinfo );
	char tmp[20], *p;
	sprintf( tmp, "%-16d", int_uid );
	p = strstr( qun_buf, tmp );
	if( p )
	{
		p -= 8;
		if( p>=qun_buf )
		{
			int ln;
			sscanf( p, "%d", &ln );
			sprintf( msgstr, "\n%d)%s{", ln, timestr );
			strcat( msgstr, qun_name );
			strcat( msgstr, "}[" );
			strcat( msgstr, nick );
			strcat( msgstr, "]\n\t" );//二次修改
			print_string( msgstr );
			puts_color( msg, CCOL_REDGREEN );
			reset_input();
		}
	}else{
		sprintf( msgstr, "\n%s{", timestr );
		strcat( msgstr, qun_name );
		strcat( msgstr, "}[" );
		strcat( msgstr, nick );
		strcat( msgstr, "]\n\t" );//二次修改
		print_string( msgstr );
		puts_color( msg, CCOL_REDGREEN );
		reset_input();
	}
}

#ifndef __WIN32__

int getch(void) 
{
    char buf = 0;
    struct termios old = {0};

    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()"); 
	
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");

    if (read(0, &buf, 1) < 0)
        perror ("read()");

    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");

    return (buf);
}

#endif

char* read_inputstr(char *pass, int maxlen, int password) 
{
    int ch;
    int passlen = 0;

    while (passlen < maxlen - 1)
    {
        ch = getch();
        if (ch == '\r' || ch == '\n')
        {
            fprintf(stderr, "\n");
            break;
        }
		
        if (ch != '\b')
        {
            pass[passlen++] = ch;
			if(password)
				fprintf(stderr, "*");
			else
				fputc(ch, stderr);

            continue;
        }
		
        if (passlen >= 1)
        {
            passlen--;
            fprintf(stderr, "\b \b");
        }
        else
        {
            fprintf(stderr, "\007");
        }
    }
	
    pass[passlen] = '\0';
	
    return pass;    
}

uint read_uid()
{
	char idstr[0x10];
	int idstrlen = sizeof(idstr);

    int ch;
    int ilen = 0;
	
    while (ilen < idstrlen - 1)
    {
        ch = getch();
        if (ch == '\r' || ch == '\n')
        {
            fprintf(stderr, "\n");
            break;
        }
		
        if (ch != '\b')
        {
			if(ch >= '0' && ch <= '9')
			{
	            fputc(ch, stderr);
				idstr[ilen++] = ch;
				continue;
			}
			else
			{
				return 0;
			}
        }
		
        if (ilen >= 1)
        {
            ilen--;
            fprintf(stderr, "\b \b");
        }
        else
        {
            fprintf(stderr, "\007");
        }
    }
	
	if(ilen > 10)
		return 0;

    idstr[ilen] = '\0';

	return atoi(idstr);
}

char read_select()
{
    char ch;
	
    for(;;)
	{
        ch = getch();
		if('y' == ch || 'n' == ch)
		{
			fputc(ch, stderr);
			return ch;
		}
		if('\r' == ch || '\n' == ch)
			return 'n';
    }
	
	return 0;
}

int main(int argc, char** argv)
{
	int cmdid, lastcmd=-1, len;
	char cmd[16], arg[1008];
	srand(time(NULL));
	//init data
	config_init();
	qqsocket_init();
	//no color?
	if( config_readint( g_conf, "NoColor" ) )
		no_color = 1;
	NEW( qun_buf, QUN_BUF_SIZE );
	NEW( buddy_buf, BUDDY_BUF_SIZE );
	NEW( print_buf, PRINT_BUF_SIZE );
	NEW( qq, sizeof(qqclient) );
	if( !qun_buf || !buddy_buf || !print_buf || !qq ){
		printf("no enough memory.\n");
		return -1;
	}
	//login
	clear_screen();

DO_LOGIN:
	if( argc < 3 )
	{
		uint uid;
		char password[32];
		print_string("QQ账号:");
		uid = read_uid();

		if( 0 == uid )
		{
			print_string("\n输入错误，重新输入。\n");
			goto DO_LOGIN;
		}

		print_string("QQ密码:");
		read_inputstr( password, sizeof(password), 1 );

		print_string("是否隐身登陆？(y/n)");
		char hidechar = read_select();
		qqclient_create( qq, uid, password );
		qq->mode = hidechar=='y' ? QQ_HIDDEN : QQ_ONLINE;
		qqclient_login( qq );
	}else{
		//check if failed here...
		qqclient_create( qq, atoi(argv[1]), argv[2] );
		if( argc > 3 )
			qq->mode = atoi(argv[3])!=0 ? QQ_HIDDEN : QQ_ONLINE;
		qqclient_login( qq );
		argc = 1;
	}
	print_string("\n登陆中...\n");
	for( ; qq->process != P_LOGIN; qqclient_wait( qq, 1 ) )
	{
		switch( qq->process ){
		case P_LOGGING:
			continue;
		case P_VERIFYING:
			print_string("请输入验证码（验证码目录下）: ");
			read_inputstr( input, sizeof(input) - 1, 0 );
			qqclient_verify( qq, input );
			break;
		case P_ERROR:
			print_string("网络错误.\n");
			qqclient_logout( qq );
			qqclient_cleanup( qq );
			goto DO_LOGIN;
		case P_DENIED:
			print_string("您的QQ需要激活(http://jihuo.qq.com).\n");
			qqclient_logout( qq );
			qqclient_cleanup( qq );
			goto DO_LOGIN;
		case P_WRONGPASS:
			print_string("您的密码错误.\n");
			qqclient_logout( qq );
			qqclient_cleanup( qq );
			goto DO_LOGIN;
		default:
			print_string("未知错误.\n");
		}
	}
	/* Success */
	print_string(help_msg);
	while( qq->process != P_INIT ){
		reset_input();
		len = inputline( input, sizeof(input) - 1 );
		if( len < 1 ) continue;
		char* sp = strchr( input, ' ' );
		if( sp ){
			*sp = '\0';
			strncpy( cmd, input, sizeof(cmd) - 1 );
			strncpy( arg, sp+1, sizeof(arg) - 1 );
			*sp = ' ';
		}else{
			strncpy( cmd, input, sizeof(cmd) - 1 );
			arg[0] = '\0';
		}
		need_reset = 1;
		for( cmdid=0; cmdid<sizeof(commands)/16; cmdid++ )
			if( strcmp( commands[cmdid], cmd )==0 )
				break;
SELECT_CMD:
		switch( cmdid ){
		case CMD_TO:
		case CMD_TO2:
		{
			if( enter )
			{
				print_string("您在一个群中, 你可以和任何人谈话.\n");
				break;
			}
			int n = atoi( arg );
			if( n < 0xFFFF )
			{
				char *p;
				p = skip_line( buddy_buf, n-1 );
				if( p )
				{
					sscanf( p, "%u%u", &n, &to_uid );
					sprintf( print_buf, "您将和 %s 进行谈话\n", myqq_get_buddy_name(qq, to_uid) );
					print_string(print_buf);
					break;
				}
			}else{
				to_uid = n;
				sprintf( print_buf, "您将和 %s 进行谈话\n", myqq_get_buddy_name(qq, to_uid) );
				print_string(print_buf);
				break;
			}
			sprintf( print_buf, "to: %s 没有找到.\n", arg );
			print_string(print_buf);
			break;
		}
		case CMD_SAY:
		case CMD_SAY2:
		{
			if( enter )
			{
				if( myqq_send_im_to_qun( qq, qun_int_uid, arg, 1 ) < 0 ){
					print_string("超时: 您的消息发送失败.\n");
				}
			}else{
				if( to_uid == 0 ){
					print_string("say: 和谁谈话?\n");
					break;
				}
				if( myqq_send_im_to_buddy( qq, to_uid, arg, 1 ) < 0 ){
					print_string("超时: 您的消息发送失败.\n");
				}
			}
			break;
		}
		case CMD_EXIT:
		case CMD_EXIT2:
			goto end;
		case CMD_HELP:
		case CMD_HELP2:
			print_string(help_msg);
			break;
		case CMD_CLS:
		case CMD_CLS2:
		case CMD_CLS3:
			clear_screen();
		case CMD_STATUS:
			if( strcmp( arg, "away") == 0 )
				qqclient_change_status( qq, QQ_AWAY );
			else if( strcmp( arg, "online") == 0 )
				qqclient_change_status( qq, QQ_ONLINE );
			else if( strcmp( arg, "hidden") == 0 )
				qqclient_change_status( qq, QQ_HIDDEN );
			else if( strcmp( arg, "killme") == 0 )
				qqclient_change_status( qq, QQ_KILLME );
			else if( strcmp( arg, "busy") == 0 )
				qqclient_change_status( qq, QQ_BUSY );
			else{
				print_string("未知状态\n");
			}
			break;
		case CMD_ENTER:
		case CMD_ENTER2:
		{
			int n = atoi( arg );
			if( n < 0xFFFF )
			{
				char *p;
				p = skip_line( qun_buf, n-1 );
				if( p )
				{
					sscanf( p, "%u%u", &n, &qun_int_uid );
					sprintf( print_buf, "您在 %s 群中\n", myqq_get_qun_name( qq, qun_int_uid) );
					print_string(print_buf);
					enter = 1;
					break;
				}
			}else{
				qun_int_uid = n;
				sprintf( print_buf, "您在 %s 群中\n", myqq_get_qun_name( qq, qun_int_uid) );
				print_string(print_buf);
				enter = 1;
				break;
			}
			sprintf( print_buf, "enter: %s 没有找到.\n", arg );
			print_string(print_buf);
			break;
		}
		case CMD_LEAVE:
		case CMD_LEAVE2:
			if( !enter )
			{
				print_string("您没有进入群.\n");
				break;
			}
			enter = 0;
			sprintf( print_buf, "离开 %s. 您将和 %s 进行谈话\n", 
				myqq_get_qun_name( qq, qun_int_uid ), myqq_get_buddy_name( qq, to_uid ) );
			print_string(print_buf);
			break;
		case CMD_QUN:
		case CMD_QUN2:
		{
			myqq_get_qun_list( qq, qun_buf, QUN_BUF_SIZE );
			print_string( qun_buf );
			break;
		}
		case CMD_UPDATE:
		case CMD_UPDATE2:
			qun_update_all( qq );
			buddy_update_list( qq );
			group_update_list( qq );
			print_string("更新中...\n");
			if( qqclient_wait( qq, 20 )<0 ){
				print_string("更新超时.\n");
			}
			break;
		case CMD_INFO:
		case CMD_INFO2:
		{
			if( !enter )
			{
				if( to_uid==0 )
				{
					print_string("请先选择一个好友.\n");
				}else{
					char* buf = (char*)malloc(1024*4);	//4kb enough!
					//update single buddy info
					buddy_update_info( qq, buddy_get( qq, to_uid, 0 ) );
					if( qqclient_wait( qq, 10 ) < 0 ){
						print_string("获取好友详细资料失败。\n");
					}
					if( myqq_get_buddy_info( qq, to_uid, buf, 1024*4 ) < 0 ){
						sprintf( print_buf, "获取 %s 的信息失败\n", myqq_get_buddy_name( qq, to_uid ) );
						print_string(print_buf);
					}else{
						print_string( buf );
					}
					free(buf);
				}
			}else{
				char* buf = (char*)malloc(1024*4);	//4kb enough!
				if( myqq_get_qun_info( qq, qun_int_uid, buf, 1024*4 ) < 0 ){
					sprintf( print_buf, "获取 %s 的信息失败\n", myqq_get_qun_name( qq, qun_int_uid ) );
					print_string(print_buf);
				}else{
					print_string( buf );
				}
				free(buf);
			}
			break;
		}
		case CMD_VIEW:
		case CMD_VIEW2:
			if( enter )
			{
				myqq_get_qun_member_list( qq, qun_int_uid, buddy_buf,
					BUDDY_BUF_SIZE, 0 );
				print_string( buddy_buf );
			}else{
				myqq_get_buddy_list( qq, buddy_buf, BUDDY_BUF_SIZE, 0 );
				print_string( buddy_buf );
			}
			break;
		case CMD_WHO:
		case CMD_WHO2:
			if( enter )
			{
				myqq_get_qun_member_list( qq, qun_int_uid, buddy_buf,
					QUN_BUF_SIZE, 1 );
				print_string( buddy_buf );
			}else{
				myqq_get_buddy_list( qq, buddy_buf, QUN_BUF_SIZE, 1 );
				print_string( buddy_buf );
			}
			break;
		case CMD_CHANGE:
		case CMD_CHANGE2:
			qqclient_logout( qq );
			qqclient_cleanup( qq );
			main( 0, NULL );
			goto end;
		case CMD_VERIFY:
		case CMD_VERIFY2:
			qqclient_verify( qq, arg );
			break;
		case CMD_ADD:
		case CMD_ADD2:
		{
			sprintf( print_buf, "添加[%d]的附言（默认空）：", atoi(arg) );
			print_string(print_buf);
			inputline( input, 50 );
			qqclient_add( qq, atoi(arg), input );
			break;
		}
		case CMD_DEL:
			qqclient_del( qq, atoi(arg) );
			break;
		default:
			//use it as the last cmd's argument
			if( lastcmd && *input )
			{
				cmdid = lastcmd;
				strncpy( arg, input, sizeof(arg)  - 1 );
				*input = 0;
				goto SELECT_CMD;
			}
			break;
		}
		lastcmd = cmdid;
	}
end:
	qqclient_logout( qq );
	qqclient_cleanup( qq );
	config_end();
	DEL( qq );
	print_string("离开。\n");
	DEL( qun_buf );
	DEL( buddy_buf );
	DEL( print_buf );
	setcolor( CCOL_NONE );
	memory_end();
	return 0;
}
