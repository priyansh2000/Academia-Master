#ifndef ADMIN_FUN
#define ADMIN_FUN


// Add necessary system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>

// Fix include paths to be relative
#include "./constants.h"
#include "../structure/faculty.h"
#include "../structure/courses.h"

// Keep the rest of the file unchanged
bool faculty_operation_handler(int connectfd);
int login_faculty(int connectfd)
{
    ssize_t rb,wb;
    char rbuffer[1000],wbuffer[1000],tempbuffer[1000];
    struct faculty ft;
    int stfd;
    wb=write(connectfd,GET_ID,strlen(GET_ID));
    if(wb==-1)
    {
        perror("There is an error while writing the error message!");
        return -1;
    }
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,rbuffer,sizeof(rbuffer));
    if (rb==-1)
    {
            perror("There is an Error while getting ID from client!");
            return -1;
    }
    int id=atoi(rbuffer);
    bzero(wbuffer,sizeof(wbuffer));
    strcpy(wbuffer,IN_PASSWORD);
    wb=write(connectfd,wbuffer,strlen(wbuffer));
    if(wb==-1)
    {
        perror("There is an error while writing the error message!");
        return -1;
    }
    //66naBvad/64sg
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,rbuffer,sizeof(rbuffer));
    stfd=open("./faculty_record.txt",O_RDWR,0744);
    if (stfd==-1)
    {
        bzero(wbuffer,sizeof(wbuffer));
        strcpy(wbuffer,ID_DOESNT_EXIT);
        strcat(wbuffer,"^");
        wb=write(connectfd,wbuffer,strlen(wbuffer));
        if (wb==-1)
        {
            perror("There is an error while sending message to client!");
            return -1;
        }
        rb=read(connectfd,rbuffer,sizeof(rbuffer)); // Dummy read
        return -1;
    }
    int offset=lseek(stfd,id*sizeof(struct faculty),SEEK_SET);
    rb=read(stfd,&ft,sizeof(struct faculty));
    close(stfd);
    if(strcmp(rbuffer,ft.password)==0)
    {
        return id;
    }
    bzero(wbuffer,sizeof(wbuffer));
    wb=write(connectfd,INVALID_LOGIN,strlen(INVALID_LOGIN));
    return -1;
}


void change_password(int connectfd,int id)
{
    char rbuffer[1000],wbuffer[1000];
    struct faculty ft;
    bzero(rbuffer,sizeof(rbuffer));
    bzero(wbuffer,sizeof(wbuffer));
    strcpy(wbuffer,NEW_PASSWORD);
    ssize_t wb,rb;
    wb=write(connectfd,wbuffer,strlen(wbuffer));
    if(wb==-1)
    {
        perror("Error writing message to the client!");
        return;
    }
    rb=read(connectfd,rbuffer,sizeof(rbuffer));
    printf("%s",rbuffer);
    if(rb==-1)
    {
        perror("Error reading from client!");
        return;
    }
    int stdfd=open("./faculty_record.txt",O_RDWR);
    int offset=lseek(stdfd,id*sizeof(ft),SEEK_SET);//here from start of the file i want to move my file descriptor
    if(offset==-1)
    {
        printf("Error");
        return;
    }
    rb=read(stdfd,&ft,sizeof(ft));
    if(rb==-1)
    {
        perror("Error while reading!");
        return;
    }
    close(stdfd);
    stdfd=open("./faculty_record.txt",O_RDWR);
    offset=lseek(stdfd,id*sizeof(struct faculty),SEEK_SET);
    strcpy(ft.password,rbuffer);
    write(stdfd,&ft,sizeof(ft));
    close(stdfd);
    wb=write(connectfd,UPDATE_SUCCESS,strlen(UPDATE_SUCCESS));
    if(wb==-1)
    {
        perror("There is an error while writing to client");
        return;
    }
    rb=read(connectfd,rbuffer,sizeof(rbuffer));//dummy read
    return;
}


void faculty_logout_exit(int connectfd)
{
    char wbuffer[1000],rbuffer[1000];
    strcpy(wbuffer,"Logging you out !$");
    write(connectfd,wbuffer,sizeof(wbuffer));
}


void add_new_course(int connectfd,int id)
{
    char rbuffer[1000],wbuffer[1000];//here this is for read write bufferr
    struct courses c;//here i am declaring courses type variable
    ssize_t rb,wb;//this is for how many bytes read and write
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // Read-write permissions for the owner and read permissions for others
    int fd=open("./courses.txt", O_RDWR|O_APPEND|O_CREAT,mode);//here i am opening the file
    bzero(rbuffer,sizeof(rbuffer));
    bzero(wbuffer,sizeof(wbuffer));
    c.f_id=id;//setting course faculty id
    sprintf(wbuffer,"%s",ADD_NAME);
    wb=write(connectfd,wbuffer,sizeof(wbuffer));//asking for course name from client
    if(wb==-1)
    {
        perror("There is an error in writing!");
        return;
    }
    rb=read(connectfd,rbuffer,sizeof(rbuffer));//reading course name from client
    if(rb==-1)
    {
        perror("There is an error while reading student name!");
        return;
    }
    strcpy(c.name,rbuffer);
    wb=write(connectfd,ADD_COURSE_ID,strlen(ADD_COURSE_ID));//asking for course id from client
    if (wb==-1)
    {
        perror("There is an error while writing the message!");
        return;
    }
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,&rbuffer,sizeof(rbuffer));//reading course id from client
    if(rb==-1)
    {
        perror("There is some error while writing ");
        return;
    }
    int course_id=atoi(rbuffer);//converting course_id to integer
    c.c_id=course_id;//assigning course_id
    wb=write(connectfd,ADD_CREDITS,strlen(ADD_CREDITS));//asking for credits
    if (wb==-1)
    {
        perror("There is an error while writing the message!");
        return;
    }
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,&rbuffer,sizeof(rbuffer));//reading credits
    if(rb==-1)
    {
        perror("There is some error while writing");
        return;
    }
    int credits=atoi(rbuffer);//converting credits to integer
    c.credit=credits;//assigning credits
    wb=write(connectfd,ADD_SEATS,strlen(ADD_SEATS));//asking for seats
    if (wb==-1)
    {
        perror("There is an error while writing the message!");
        return;
    }
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,&rbuffer,sizeof(rbuffer));//reading credits
    if(rb==-1)
    {
        perror("There is some error while writing");
        return;
    }
    int seats=atoi(rbuffer);
    c.seats=seats;//assigning seats this is total seats
    c.avail_seats=seats;//at starting avail_seats will be equal to total seats
    write(fd,&c,sizeof(c));//writing the enrolled course into the enrolled file
    close(fd);
    wb=write(connectfd,COURSE_SUCCESS,strlen(COURSE_SUCCESS));//giving success message course has been successfully added
    if(wb==-1)
    {
        perror("There is an error while writing to client");
        return;
    }
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,rbuffer,sizeof(rbuffer));//dummy read
    return;
}

void view_courses(int connectfd, int id) 
{
    ssize_t rb, wb;//this is for number read and write bytes
    char wbuffer[1000],rbuffer[1000]; //Buffer for writing
    struct courses c; //Variable for storing course data
    int fd = open("./courses.txt", O_RDONLY); //Open the courses file in read-only mode
    if (fd==-1) 
    {
        perror("Error opening courses file for reading");
        return;
    }
    while (read(fd,&c,sizeof(c))>0)
    {
        if (c.f_id == id) //here i am checking if faculty id matches 
        {
            bzero(wbuffer,sizeof(wbuffer));
            sprintf(wbuffer,"Course Details:\n\tCourse ID: %d\n\tName: %s\n\tCredits: %d\n\tSeats: %d\n\tAvailable seats:%d", c.c_id, c.name, c.credit,c.seats,c.avail_seats);
            strcat(wbuffer,"\n\nYou'll now be redirected to the main menu...^");
            wb = write(connectfd, wbuffer, strlen(wbuffer));//Send course information to the client
            if (wb == -1) 
            {
                perror("Error writing course information to the client");
                return;
            }
            bzero(rbuffer,sizeof(rbuffer));
            rb=read(connectfd,rbuffer,sizeof(rbuffer)); // Dummy read
        }
    }
    close(fd);
}

void remove_course(int connectfd,int id)
{
    ssize_t rb,wb;
    char rbuffer[1000],wbuffer[1000];
    struct courses c;
    bzero(rbuffer,sizeof(rbuffer));
    bzero(wbuffer,sizeof(wbuffer));
    wb=write(connectfd,ADD_COURSE_ID,strlen(ADD_COURSE_ID));
    if(wb==-1)
    {
        perror("There is an error while writing the message!");
        return;   
    }
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,rbuffer,sizeof(rbuffer));
    if(rb==-1)
    {
        perror("There is an error while reading the message!");
        return;
    }
    int course_id=atoi(rbuffer);
    int fd = open("./courses.txt",O_RDWR);
    if (fd==-1) 
    {
        perror("Error opening courses file for reading");
        return;
    }
    while(read(fd,&c,sizeof(c))>0)
    {
        if (c.f_id==id&&c.c_id==course_id) //here i am checking if faculty id matches 
        {
            lseek(fd, -sizeof(c), SEEK_CUR);//now i am moving the file offset to overwrite
            c.f_id=-1;
            write(fd, &c, sizeof(c)); //writing the updated course back to the file
            close(fd);
            wb=write(connectfd,UPDATE_SUCCESS,strlen(UPDATE_SUCCESS));
            if(wb==-1)
            {
                perror("There is an error while writing to client");
                return;
            }
            rb=read(connectfd,rbuffer,sizeof(rbuffer));//dummy read
            return;
        }
    }
    return;
}


bool faculty_operation_handler(int connectfd)
{
    int id=login_faculty(connectfd);
    if (id!=-1)//if login successfull then we have to do this
    {
        ssize_t wbytes,rbytes;            
        char rbuffer[1000], wbuffer[1000];
        bzero(wbuffer,sizeof(wbuffer));
        bzero(rbuffer,sizeof(rbuffer));
        while (1)
        {
            strcat(wbuffer,"\n");
            strcat(wbuffer,FACULTY_MENU);
            wbytes=write(connectfd,wbuffer,strlen(wbuffer));
            if(wbytes==-1)
            {
              perror("There is some erron on writing!");
              return false;
            }
              bzero(wbuffer,sizeof(wbuffer));
              bzero(rbuffer,sizeof(rbuffer));
              rbytes=read(connectfd,rbuffer,sizeof(rbuffer));
              if(rbytes==-1)
             {
                perror("There is an error while reading MENU");
                return false;
             }
            int choice=atoi(rbuffer);
            switch(choice)
            {
                case 1:
                view_courses(connectfd,id);
                break;
                case 2:
                add_new_course(connectfd,id);
                break;
                case 3:
                remove_course(connectfd,id);
                break;
                case 4:
                break;
                case 5:
                change_password(connectfd,id);
                break;
                case 6:
                faculty_logout_exit(connectfd);
                break;
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}


#endif
