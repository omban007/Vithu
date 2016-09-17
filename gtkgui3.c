#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <malloc.h>
#include <linux/reboot.h>
#include <libtar.h>
#include <time.h>
#include <sys/utsname.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include "jardir.h"

char *str=NULL;
char sfile[1000][1000];
char filepath[1024];
char copypath[1024],copyfile[1024];
int avail=0;
TAR *pTar;


void addfile(char *string)
{
static int i=0;
strcpy(sfile[i],string);
i++;
}

void jarsay(char *str)
{
FILE *fp;
fp=fopen("jarsay.txt","w+");
if(fp==NULL)
{
        fprintf(stderr,"sorry jarsay.txt file is not open\n");
}
fputs(str,fp);
fclose(fp);
system("text2wave jarsay.txt > jarsay.wav");
system("aplay jarsay.wav");
}

void stringcut(char *ch,char *stringm)
{
char dh[50][50];
str=malloc(sizeof(char*));
int i=0,j=0;

while(*ch!='\0')
{
str[i++]=*ch;
strcpy(dh[j],str);
if(*ch==' ')
{
str[i]='\0';
dh[j][i]='\0';
i=0;
j++;
}
*ch++;

}
dh[j][i]='\0';

for(i=0;i<=j;i++)
{
if(!strcmp(dh[i],stringm))
{
strcpy(str,dh[i+1]);
}
}
}

char * arandom()
{
char str[1024];
char *string;
string=(char*)malloc(sizeof(char*)*1024);
FILE *fp;
char ch;
int i=0,line=0;

fp=fopen("arandom.txt","a+w");
if(fp==NULL)
{
        fprintf(stderr,"arandom.txt file not found sar\n");
}

while(ch!=EOF)
{
        ch=fgetc(fp);
        if(ch=='\n')
        {
                line++;
        }
}

for(i=0;i<line;i++)
{
random();
}

snprintf(str,sizeof(str),"%ld",random());
fputs(str,fp);
fputs("\n",fp);
strcpy(string,str);
strcat(string,".bmp");

return string;
}



void listdir(const char *name,int level,char *filename)
{
DIR *dir;
struct dirent *entry;

if(!(dir=opendir(name)))
        return;
if(!(entry=readdir(dir)))
        return;

do{
if(entry->d_type==DT_DIR)
{
char path[1024];
int len=snprintf(path,sizeof(path)-1,"%s/%s",name,entry->d_name);
path[len]=0;

if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)
        continue;

snprintf(filepath,sizeof(filepath)-1,"%s/",path);

listdir(path,level+1,filename);
}
else
{
        if(!strcmp(entry->d_name,filename))
        {       strcat(filepath,filename);
                printf("%s\n",1+filepath);
                avail++;
        }
}

}while(entry=readdir(dir));

closedir(dir);
}

void hidefile(char *fname)
{
char *comm;
comm=malloc(sizeof(char*)*4);
strcpy(comm,".");
strcat(comm,fname);
if(rename(fname,comm)==0)
{
        printf("%s ........file is hided\n",fname);
}
else
{
        printf("%s ........sorry sir file is not hide\n",fname);
}

}


void jargreet()
{
time_t t=time(NULL);
struct tm tm=*localtime(&t);

if(tm.tm_hour>=4&&tm.tm_hour<12)
{       
        jarsay("Good morning sar");
}
else if(tm.tm_hour>=12&&tm.tm_hour<16)
{       
        jarsay("Good afternoon sar");

}
else if(tm.tm_hour>=16&&tm.tm_hour<20)
{       
        jarsay("Good evening sar");
}
}


int scanbluetooth()
{
inquiry_info *ii = NULL;
int max_rsp, num_rsp;
int dev_id, sock, len, flags;
int i,dev=0;
char addr[19] = { 0 };
char name[248] = { 0 };
dev_id = hci_get_route(NULL);
sock = hci_open_dev( dev_id );

if (dev_id < 0 || sock < 0)
{
        perror("opening socket");
        //exit(1);
}

len = 8;
max_rsp = 255;
flags = IREQ_CACHE_FLUSH;
ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
if( num_rsp < 0 ) perror("hci_inquiry");
for (i = 0; i < num_rsp; i++)
{
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name),
        name, 0) < 0)
        strcpy(name, "[unknown]");
        printf("%s %s\n", addr, name);
	dev++;
}

if(dev==0)
{
	printf("No Device Found sir\n");
	jarsay("No Device Found sar");
}

free( ii );
close( sock );
return 0;
}

void showhiddenall()
{
DIR *dp;
int num=0;
struct dirent *result;
dp=opendir((char*)get_current_dir_name());

if(dp==NULL)
{
        perror("");
}
result=readdir(dp);

while(result!=NULL)
{
if(result->d_name[0]=='.')
        if(result->d_type==DT_DIR)
                printf(ANSI_COLOR_BLUE "%d %s\n" ANSI_COLOR_RESET,num,result->d_name);
        else
                printf("%d %s\n",num,result->d_name);
num++;
result=readdir(dp);
}

return;
}


int ChangeDir(char *string)
{
DIR *dp;
int num=1,check=0;
struct dirent *result;
dp=opendir((char*)get_current_dir_name());

if(dp==NULL)
{
perror("");
return -1;
}
result=readdir(dp);

while(result!=NULL)
{
if(result->d_name[0]!='.')
{
if(result->d_type==DT_DIR)
{
if(atoi(string)==0)
{
        if(!strcasecmp(result->d_name,string))
                {
                chdir(result->d_name);
                check++;
                }
}
else
{
        if(atoi(string)+1==num)
        {
                chdir(result->d_name);
                check++;
        }
}
}
}

num++;
result=readdir(dp);
}

if(check==0)
{
printf("Directory is not found sir\n");
jarsay("Directory is not found sar");
//return 1;
}
else
{
printf("Directory is changed sir\n");
printf("cwd: %s\n",(char*)get_current_dir_name());
jarsay("Directory is changed sar");
//return 0;
}
}

void jartime()
{
        time_t t=time(NULL);
        struct tm tm=*localtime(&t);
        printf("Sir the Time is: %d:%d:%d\n",tm.tm_hour,tm.tm_min,tm.tm_sec);
        return;
}


void backdir()
{
int ret=0;
ret=chdir((char*)dirname((char*)get_current_dir_name()));

if(ret==-1)
{
        printf("can't get back");
	jarsay("Cant get back sar this might be last folder");
}

printf("cwd:%s\n",(char*)get_current_dir_name());
}


int CreateTar(char *tarname)
{
	int ret=0;

	ret=tar_open(&pTar,tarname, NULL, O_WRONLY | O_CREAT, 0644, TAR_GNU);
	if(ret!=0)
	{
		fprintf(stderr,"File is not Created sir\n");
		//return -1;
	}

	return 0;
}

int TarAppend(char *tarfile)
{
	if(tar_append_file(pTar,tarfile,tarfile)!=0)
	{
		fprintf(stderr,"File is Not Appended sir\n");
		//return -1;
	}

	return 0;
}

void TarClose()
{
	tar_append_eof(pTar);
	tar_close(pTar);
	//return;
}

int hackfile(char *argv)
{
	char buf[1024];

	strcpy(buf,"exiftool ");
	strcat(buf,argv);

	//printf("Here the meta info about the file sir\n");
	printf("........................................\n");
	system(buf);

	return 0;
}

int eject()
{
	int cdrom;
	int i;

	if ((cdrom = open("/dev/sr0",O_RDONLY | O_NONBLOCK)) < 0) {
	perror("open");
	exit(1);
	}
	if(ioctl(cdrom, CDROMEJECT) < 0) {
		printf("Could not eject cdrom tray\n");
		jarsay("could not eject cdrom sar");
	}

	return 0;
}

void SetAlsaMasterVolume(long volume)
{
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
}

void getsysinfo()
{
struct utsname sysinfo;

if(uname(&sysinfo)==0)
{
	printf("sysname:  %s\n",sysinfo.sysname);
	printf("nodename: %s\n",sysinfo.nodename);
	printf("release:  %s\n",sysinfo.release);
	printf("version:  %s\n",sysinfo.version);
	printf("machine:  %s\n",sysinfo.machine);
}

return;
}

void newsnotify()	
{
int i;

for(;;)
{
	system("python ~/jarvis/JarNewsNotify.py &");
	sleep(1800);
}

}


void enter_callback(GtkWidget *widget,GtkWidget *entry)
{
DIR *dp;
struct dirent *result;
static int flag,check;
int len=0,i,num=0;
char *camimg;
const gchar *entry_text;
camimg=(char*)malloc(sizeof(char*)*1024);
entry_text=gtk_entry_get_text(GTK_ENTRY(entry));
//printf("Entry contents:%s\n",entry_text);
if(!strcmp(entry_text,"file mode on"))
{
	flag=1;
	printf("File mode activated sir..\n");
	jarsay("File mode is activated sar");
}
else if(!strcmp(entry_text,"file mode off"))
{
	flag=0;
	printf("File mode deactivated sir..\n");
	jarsay("File mode deactivated sar");
}

else if(!strcmp(entry_text,"command mode on")||!strcmp(entry_text,"activate command mode"))
{
	flag=2;
	printf("Command mode activated sir..\n");
	jarsay("Command mode activated sar");
}
else if(!strcmp(entry_text,"command mode off")||!strcmp(entry_text,"deactivate command mode"))
{
	printf("Command mode deactivated sir\n");
	jarsay("Command mode deactivated sar");
	flag=0;
}

if(flag==2)
{
	printf("..................................................................................\n");
	system((char*)entry_text);
}

if(flag==0)
{

if(strstr(entry_text,"look at me")||strstr(entry_text,"camera"))
{
	jarsay("Yes sar am looking at you");
	system("cheese &");
}

if(strstr(entry_text,"capture")||strstr(entry_text,"photo")||strstr(entry_text,"cheese"))
{
	printf("sir smile please\n");
	strcpy(camimg,"python jarcam.py ");
	strcat(camimg,arandom());
	system(camimg);
}


if(strstr(entry_text,"system")||strstr(entry_text,"system detail")||strstr(entry_text,"os"))
{
	getsysinfo();
}

if(strstr((char*)entry_text,"reboot")||strstr((char*)entry_text,"restart"))
{
	jarsay("saving all data and rebooting system sar");
	sync();
	reboot(LINUX_REBOOT_CMD_RESTART);
}

if(strstr(entry_text,"shutdown")||strstr(entry_text,"good night"))
{
	time_t t=time(NULL);
	struct tm tm=*localtime(&t);
	printf("Good night sir\n");
	if(tm.tm_hour>=20&&tm.tm_hour<24)
	{       
		jarsay("Good night sar\n");
	}
	sync();
	reboot(LINUX_REBOOT_CMD_POWER_OFF);
}

if(strstr((char*)entry_text,"time"))
{
jartime();
}

if(!strcmp("play song",entry_text)||!strcmp("jarvis play song",entry_text)||!strcmp("jarvis play audio song",entry_text) || !strcmp("play audio song",entry_text))
{
	system("nohup vlc /media/hacker/Multimedia/myfvrt/*.mp3 &");
}

if(!strcmp("show me movies",entry_text))
{
	system("xdg-open /media/hacker/Multimedia/Movie/");
}

if(strstr(entry_text,"play movie")||strstr(entry_text,"play movies"))
{
system("vlc /media/hacker/Multimedia/Movie/*.*");
}

if(strstr(entry_text,"video"))
{
system("vlc /media/hacker/Multimedia/Video_song/Hindi_video/*.mp4");
}

if(strstr(entry_text,"facebook"))
{
	printf("Please wait for facebook feed sir........\n");
	jarsay("please wait for facebook feed sar");
	system("ruby ./facefeed.rb");
}

if(strstr(entry_text,"twitter"))
{
	printf("Please wait for twitter feed sir........\n");
	jarsay("Please wait for twitter feed sar");
	system("python ./twittime.py");
}

if(strstr(entry_text,"news"))
{
	printf("Please wait for news sir......\n");
	jarsay("please wait for news sar");
	system("ruby news.rb");
}

if(strstr(entry_text,"eject"))
{
	eject();
}

if(strstr(entry_text,"scan")||strstr(entry_text,"bluetooth"))
{	
	printf("Bluetooth Devices Scanning started........\n");
	jarsay("Bluetooth devices scanning started");
	scanbluetooth();
	printf("Bluetooth Devices Scanning stopped........\n");
	jarsay("Bluetooth deviced scanning stopped");
}

if(strstr(entry_text,"volume"))
{
	stringcut((char*)entry_text,"at ");
	if(str==NULL)
	{
		stringcut((char*)entry_text,"volume ");
	}
	SetAlsaMasterVolume(atoi(str));
	printf("Volume is changed sir\n");	
	jarsay("Volume changed sar");
}

if(strstr(entry_text,"mute"))
{
	SetAlsaMasterVolume(0);
}

if(strstr(entry_text,"unmute"))
{
	SetAlsaMasterVolume(50);
	jarsay("Hi sar");
}

if(strstr(entry_text,"fuck"))
{
jarsay("please control your toung sar");
}

}
else if(flag==1)//file mode operation start
{
	dp=opendir((char*)get_current_dir_name());
	if(dp==NULL)
	{
        	perror("");
        	exit(1);
	}
	
	if(!strcmp(entry_text,"select all"))
	{
		result=readdir(dp);

		while(result!=NULL)
		{
			if(result->d_name[0]!='.'){
        			addfile(result->d_name);
        			check++;}
		result=readdir(dp);
		}		

		//return check;
	}

	else if(!strcmp(entry_text,"select all files only") || !strcmp(entry_text,"select all file only") || !strcmp(entry_text,"select all only files") || !strcmp(entry_text,"select all only file"))
	{
	result=readdir(dp);

	while(result!=NULL)
	{
	if(result->d_type!=DT_DIR)
        	if(result->d_name[0]!='.'){
                	addfile(result->d_name);
                	check++;}
	result=readdir(dp);
	}
	}

	else if(!strcmp(entry_text,"select all folders only") || !strcmp(entry_text,"select all folder only") || !strcmp(entry_text,"select all only folders") || !strcmp(entry_text,"select all only folder"))
	{
	result=readdir(dp);

	while(result!=NULL)
	{
	if(result->d_type==DT_DIR)
        	if(result->d_name[0]!='.'){
                	addfile(result->d_name);
               		check++;}
	result=readdir(dp);
	}
	}
	
	else if(!strcmp(entry_text,"select files"))
	{
		check=0;
	}

	result=readdir(dp);
	while(result!=NULL)
	{
	//if(result->d_name[0]!='.')
	{
	if(atoi(entry_text)!=0)
	{
        	if(atoi(entry_text)==num)
                	{
                	addfile(result->d_name);
          	        //printf("the file is matched sir%d %s\n",num,result->d_name);
                	check++;
                	}
	}

	else

	{
        	if(!strcasecmp(strrcut(result->d_name),entry_text))
                	{
                	addfile(result->d_name);
                	//printf("the file is matched sir %s\n",result->d_name);
                	check++;
                	}

	}	
	}
	num++;
	result=readdir(dp);
	}
	rewinddir(dp);

	//printf("selected files %d\n",check);
	if(!strcmp(entry_text,"show selected files"))
	{
	//printf("........................................................................................................................................................................\n");
	printf("SELECTED FILES\n");
	printf(".........................................................................................................................................................................\n");
	for(i=0;i<check;i++)
	{
		printf("%s\n",sfile[i]);
	}}

	if(!strcmp(entry_text,"show me files")||!strcmp(entry_text,"show all")||!strcmp(entry_text,"list directory")||!strcmp(entry_text,"show files"))
	{
		ShowFileWithNum();
	}

	if(strstr(entry_text,"change")||strstr(entry_text,"cd"))
	{
		stringcut((char*)entry_text,"to ");
		if(str==NULL)
		{
			 stringcut((char*)entry_text,"into ");
		}

		if(str==NULL)
                {
                         stringcut((char*)entry_text,"cd ");
                }

		if(str==NULL)
                {
                         stringcut((char*)entry_text,"in ");
                }

		ChangeDir(str);
		//printf("%s\n",str);
	}

	if(strstr(entry_text,"show")&&strstr(entry_text,"hidden")||strstr(entry_text,"hided"))
	{
		showhiddenall();
	}

	if(strstr(entry_text,"current")||strstr(entry_text,"working"))
	{
		jarsay("here curren working directory sar");
		printf("Current directory is %s\n",(char*)get_current_dir_name());
	}
	
	if(strstr(entry_text,"compress")||strstr(entry_text,"archive"))
	{
		printf("Compressing started.....\n");
		jarsay("Compressing stared sar");
		stringcut((char*)entry_text,"to ");

		if(str==NULL)
                {
                         stringcut((char*)entry_text,"into ");
                }

                if(str==NULL)
                {
                         stringcut((char*)entry_text,"in ");
                }
		strcat(str,".tar");
		printf("Compressing files to %s.............\n",str);
		CreateTar(str);
		for(i=0;i<check;i++)
		{
			TarAppend(sfile[i]);
		}
		printf("Compressing completed sir.\n");
		jarsay("Compressing completed sar");
		//printf("%s\n",str);
	}

	if(strstr(entry_text,"hack") || strstr(entry_text,"meta"))
	{

		printf("Here the meta info about the file sir\n");
		jarsay("here file hack sar");
		printf("........................................\n");

		for(i=0;i<check;i++)
		{
			hackfile(sfile[i]);
		}
	}

	if(strstr(entry_text,"detail")||strstr(entry_text,"details")||strstr(entry_text,"info")||strstr(entry_text,"information"))
	{
		printf("Here detail about the file sir\n");
		jarsay("here detail about the file sar");
		printf("................................................\n");
		for(i=0;i<check;i++)
		{
			showdetail(sfile[i]);
		}
	}

	if(strstr(entry_text,"remove")||strstr(entry_text,"delete"))
	{
		printf("file is deleting sir...................\n");
		jarsay("file is deleting sar");
		for(i=0;i<check;i++)
		{
			if(opendir(sfile[i])!=NULL)
			{
				printf("%s ...folder are deleting sir\n",sfile[i]);
				if(rmdir(sfile[i])==-1)
				{
					printf("Unable to delete the directory: %s\n",sfile[i]);
					jarsay("This folder is not empty sar i cant delete it");
				}
			}
			else
			{
				printf("%s .....file is deleting sir\n",sfile[i]);
				unlink(sfile[i]);
			}
		}
		printf("delete existing....\n");
		jarsay("delete existing");
	}

	if(strstr(entry_text,"open"))
	{
		char *scommand;
		scommand=malloc(sizeof(char*));
		strcpy(scommand,"xdg-open ");
		stringcut((char*)entry_text,"open ");
		strcat(scommand,str);	
		system(scommand);
	}

	if(strstr(entry_text,"search"))
	{
		printf("File is searching please wait......\n");
		jarsay("Am searching file sar please wait");
		stringcut((char*)entry_text,"search ");
		if(str==NULL)
		{
			stringcut((char*)entry_text,"file ");
		}
		
		if(str!=NULL)
		{
			listdir("/",0,str);
		}
		printf("File searching completed........\n");
		jarsay("file search completed sar");
	}
	
	if(strstr(entry_text,"create")||strstr(entry_text,"make"))
	{
		stringcut((char*)entry_text,"directory ");
		if(str==NULL)
		{
			stringcut((char*)entry_text,"folder ");
		}
		if(mkdir(str,777)==0)
		{
			printf("%s Directory is created sir\n",str);
			jarsay("Directory is created sar");
		}
		else
		{			
			fprintf(stderr,"Directory is not created sir\n");
			jarsay("sorry sar i can't create this directory");
		}
	}
	


	if(!strcmp(entry_text,"hide file")||!strcmp(entry_text,"hide files")||!strcmp(entry_text,"hide all"))
	{
		printf("File is hiding started....\n");
		jarsay("file is hiding sar");
		for(i=0;i<check;i++)
		{
	            hidefile(sfile[i]);
		}
		printf("Files are hiding stopped...\n");	
		jarsay("all files are hided sar");
	}
	
	if(strstr(entry_text,"back"))
	{
		backdir();
	}

	if(strstr(entry_text,"copy"))
	{
		static int once;
		if(once==0)
		{
			strcpy(copypath,(char*)get_current_dir_name());
			strcpy(copyfile,(char*)get_current_dir_name());
			once++;
		}
		//printf("%s\n",copypath);
	}

	if(strstr(entry_text,"pest"))
	{
		printf("File Copying started...\n");
		jarsay("file copying started sar");
		for(i=0;i<check;i++)
		{
			strcat(copypath,"/");
			strcat(copypath,sfile[i]);
			//strcpy(copypath,copyfile);
			CopyFile(copypath,(char*)get_current_dir_name());
			strcpy(copypath,copyfile);
		}
		printf("%d file are copied\n",check);
		jarsay("files are copyied sar");
	}
	//printf("%s\n",entry_text);
}
}

int graphics()
{
GtkWidget *window;
GtkWidget *vbox,*hbox;
GtkWidget *entry;
GtkWidget *button;
GdkColor color;
color.red = 0x00C0;
color.green = 0x00DE;
color.blue = 0x00ED;
//GtkWidget *check;

//gtk_init(&argc,&argv);

window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_widget_set_usize(GTK_WIDGET(window),300,40);
gtk_window_set_title(GTK_WINDOW(window),"JARVIS");
gtk_signal_connect(GTK_OBJECT(window),"delete_event",(GtkSignalFunc)gtk_exit,NULL);

vbox=gtk_vbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(window),vbox);
gtk_widget_show(vbox);

entry=gtk_entry_new_with_max_length(200);
gtk_signal_connect(GTK_OBJECT(entry),"activate",GTK_SIGNAL_FUNC(enter_callback),entry);
gtk_entry_set_text(GTK_ENTRY(entry),"Hellow");
gtk_entry_append_text(GTK_ENTRY(entry)," sir");
gtk_entry_select_region(GTK_ENTRY(entry),0,GTK_ENTRY(entry)->text_length);
gtk_box_pack_start(GTK_BOX(vbox),entry,TRUE,TRUE,0);
gtk_widget_show(entry);

hbox=gtk_hbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(vbox),hbox);
gtk_widget_show(hbox);

//gtk_widget_show(button);

gtk_widget_set_uposition( window, 1200, 670 );
//gtk_window_set_decorated(GTK_WINDOW(window),FALSE);
gtk_window_stick(GTK_WINDOW(window));
gtk_window_set_keep_above(GTK_WINDOW(window),TRUE);
gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
gtk_window_unfullscreen(GTK_WINDOW(window));
//gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
gtk_widget_show(window);
gtk_main();
return 0;
}

int main(int argc,char *argv[])
{
gtk_init(&argc,&argv);

jargreet();
if(fork()>0)
{
graphics();
}
//else
{
//newsnotify();//temporary avoided while runing program again and again
}
//printf("%s\n",stringcut("jarvis select om folder"));
//stringcut("jarvis show me folder","show ");
//printf("%s\n",str);
//printf("Hellow world\n");
return 0;
}
