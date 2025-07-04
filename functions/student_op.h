#ifndef STUDENT_FUN
#define STUDENT_FUN

// Include necessary system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>

// Fix relative includes
#include "./constants.h"
#include "../structure/student.h"
#include "../structure/courses.h"
#include "../structure/enroll.h"

bool student_operation_handler(int connectfd);
int login_student(int connectfd);
void change_password(int connectfd,int id);
int login_student(int connectfd)
{
    ssize_t rb,wb;
    char rbuffer[1000],wbuffer[1000],tempbuffer[1000];
    struct student ft;
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
    stfd=open("./student_record.txt",O_RDWR);
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
    int offset=lseek(stfd,id*sizeof(struct student),SEEK_SET);
    rb=read(stfd,&ft,sizeof(struct student));
    close(stfd);
    if(strcmp(rbuffer,ft.password)==0)
    {
        return id;
    }
    bzero(wbuffer,sizeof(wbuffer));
    wb=write(connectfd,INVALID_LOGIN,strlen(INVALID_LOGIN));
    return -1;
}


void change_student_password(int connectfd,int id)
{
    ssize_t rb,wb;//this is for counting number of bytes read and write
    char rbuffer[1000],wbuffer[1000];//this is for buffer or to store
    struct student st;//here we are declaring variable of struct type
    int stfd;//student file descriptor
    stfd=open("./student_record.txt",O_RDWR);
    int offset=lseek(stfd,id*sizeof(struct student),SEEK_SET);//means from begining we have to move this
    if(offset==-1)//means there is some error in seeking
    {
        return;
    }
    rb=read(stfd,&st,sizeof(st));//here we are reading that specific record that password we have to change
    if(rb==-1)//means there is some error in reading
    {
        return;
    }
    bzero(wbuffer,sizeof(wbuffer));
    strcpy(wbuffer,NEW_PASSWORD);
    wb=write(connectfd,wbuffer,strlen(wbuffer));
    if(wb==-1)//means there is some error in writing
    {
        return;
    }
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,rbuffer,sizeof(rbuffer));//we are here reading new password from client
    if(rb==-1)//measn there is some error in reading from the client
    {
        return;
    }
    strcpy(st.password,rbuffer);
    offset=lseek(stfd,-sizeof(struct student),SEEK_CUR);//here we are moving back to that record position as after read offset will be set after that record so we are moving it back
    if(offset==-1)//there is some error in setting the offset
    {
        return;
    }
    wb=write(stfd,&st,sizeof(st));//we are writing that record to student record
    if(wb==-1)//there is some error in writing
    {
        return;
    }
    close(stfd);
     wb=write(connectfd,UPDATE_SUCCESS, strlen(UPDATE_SUCCESS));
    if(wb==-1)//there is some error writing this error message
     {
         return;
    }
    rb=read(connectfd,rbuffer,sizeof(rbuffer));//dummy read
    return;
}


void student_logout_exit(int connectfd)
{
    char wbuffer[1000],rbuffer[1000];
    strcpy(wbuffer,"Logging you out !$");
    write(connectfd,wbuffer,sizeof(wbuffer));
}


void view_all_courses(int connectfd)
{
    ssize_t rb,wb;
    char rbuffer[1000],wbuffer[1000];
    struct courses c;
    int fd=open("./courses.txt",O_RDONLY);
    if (fd==-1) 
    {
        perror("Error opening courses file for reading");
        return;
    }
    while (read(fd,&c,sizeof(c))>0)
    {
            bzero(wbuffer,sizeof(wbuffer));
            sprintf(wbuffer,"Course Details:\n\tCourse ID: %d\n\tName: %s\n\tCredits: %d\n\tSeats: %d\n\tAvailable seats:%d", c.c_id, c.name, c.credit,c.seats,c.avail_seats);
            strcat(wbuffer,"\n\n...^");
            wb=write(connectfd, wbuffer, strlen(wbuffer));//Send course information to the client
            if (wb == -1) 
            {
                perror("Error writing course information to the client");
                return;
            }
            bzero(rbuffer,sizeof(rbuffer));
            rb=read(connectfd,rbuffer,sizeof(rbuffer)); // Dummy read
    }
    close(fd);
}


void enroll_courses(int connectfd,int id)
{
    ssize_t rb,wb;//here we are defining the number of read bytes and write bytes
    char rbuffer[1000],wbuffer[1000];//this is for read buffer and write buffer
    struct enrolled en;//here we are defining the enroll type variable
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // Read-write permissions for the owner and read permissions for others
    int fd=open("./enrolled.txt", O_CREAT|O_RDWR|O_APPEND,mode);
    int cfd=open("./courses.txt",O_RDWR);
    bzero(rbuffer,sizeof(rbuffer));
    bzero(wbuffer,sizeof(wbuffer));
    en.sid=id;
    wb=write(connectfd,ADD_COURSE_ID,strlen(ADD_COURSE_ID));//asking for course id in which student want to enroll
    if(wb==-1)
    {
        perror("There is an error while writing the message!");
        return;
    }
    bzero(rbuffer,sizeof(rbuffer));
    rb=read(connectfd,&rbuffer,sizeof(rbuffer));
    if(rb==-1)
    {
        perror("There is some error in reading");
        return;
    }
    struct courses crs;
    int course_id=atoi(rbuffer);
    int found=0;
    while(read(cfd,&crs,sizeof(crs))>0)
    {
        if(crs.c_id==course_id)
        {
            if(crs.avail_seats>0)
            {
                found=1;
                break;
            }
            else
            break;
        }
    }
    if(found==1)
    {
    crs.avail_seats=crs.avail_seats-1;
    int offset=lseek(cfd,-sizeof(struct courses),SEEK_CUR);//here we are moving back to that record position as after read offset will be set after that record so we are moving it back
    write(cfd,&crs,sizeof(crs));
    close(cfd);
    en.cid=course_id;
    wb=write(fd,&en,sizeof(en));
    close(fd);
    wb=write(connectfd,COURSE_SUCCESS,strlen(COURSE_SUCCESS));//giving success message course has been successfully added
    if(wb==-1)
    {
        perror("There is an error while writing to client");
        return;
    }
    rb=read(connectfd,rbuffer,sizeof(rbuffer));//dummy read
    return;
    }
    else
    {
        
        wb=write(connectfd,COURSE_FAILURE,strlen(COURSE_FAILURE));//giving success message course has been successfully added
        if(wb==-1)
        {
           perror("There is an error while writing to client");
           return;
        }
        rb=read(connectfd,rbuffer,sizeof(rbuffer));//dummy read
        return;
    }
}


void drop_course(int connectfd,int id)
{
       ssize_t rb,wb;//here we are defining the number of write bytes and read bytes
       char rbuffer[1000],wbuffer[1000];//this is for read and write buffer
       struct enrolled en;//here we are defining the enrolled type variable
       int fd=open("./enrolled.txt",O_RDWR);//we are opening the file in read write mode
       struct courses c;//here we are defining the courses type variable c
       int cd=open("./courses.txt",O_RDWR);//this is for opeing the courses in read write mode
       bzero(rbuffer,sizeof(rbuffer));
       bzero(wbuffer,sizeof(wbuffer));
       wb=write(connectfd,ADD_COURSE_ID,strlen(ADD_COURSE_ID));//asking for course id in which student want to drop
       if(wb==-1)
       {
          perror("There is an error while writing the message!");
          return;
       }
       bzero(rbuffer,sizeof(rbuffer));
       rb=read(connectfd,&rbuffer,sizeof(rbuffer));//this is for reading the rbuffer
       if(rb==-1)
       {
          perror("There is some error in reading");
          return;
       }
       int course_id=atoi(rbuffer);//this is for converting the read buffer content into integer
       while(read(fd,&en,sizeof(en))>0)//we are checking in enrolled file
       {
           if(en.cid==course_id)//means if we found course id to give id
           {
              while(read(cd,&c,sizeof(c))>0)//we are checking in course file
              {
                if(c.c_id==course_id)
                {
                    c.avail_seats=c.avail_seats+1;//we increase the seat by 1
                    int offset=lseek(cd,-sizeof(struct courses),SEEK_CUR);//means from current we are going behind
                    write(cd,&c,sizeof(c));//writing the data into the file
                    break;
                }
              }
           }
           en.sid=-1;//we are setting sid to -1 which is id of no one means no one has taken this course
           int offset=lseek(fd,-sizeof(struct enrolled),SEEK_CUR);
           write(fd,&en,sizeof(en));//writing this into file
           close(fd);//closing the file descriptor
           close(cd);//closing the file descriptor
           wb=write(connectfd,COURSE_SUCCESS_REMOVE,strlen(COURSE_SUCCESS_REMOVE));//giving success message course has been successfully DELETE
           if(wb==-1)
           {
               perror("There is an error while writing to client");
               return;
           }
          rb=read(connectfd,rbuffer,sizeof(rbuffer));//dummy read
          return;
       }
       wb=write(connectfd,COURSE_FAILURE,strlen(COURSE_FAILURE));//giving success message course has been successfully added
       if(wb==-1)
       {
           perror("There is an error while writing to client");
           return;
       }
       rb=read(connectfd,rbuffer,sizeof(rbuffer));//dummy read
       return;
}


void view_enroll_course(int connectfd,int id)
{
    ssize_t rb,wb;//here we are defining the number of write bytes and read bytes
    char rbuffer[1000],wbuffer[1000];//this is for read and write buffer
    struct enrolled en;//this is for enrolled courses
    int fd=open("./enrolled.txt",O_RDONLY);//here we are opening the course in read only mode
    struct courses c;//here we are defining the variable for course c
    int cd=open("./courses.txt",O_RDONLY);//here we are opening the course in read only mode
    while(read(fd,&en,sizeof(en))>0)
    {
        if(en.sid==id)
        {
            int offset=lseek(cd,sizeof(struct courses),SEEK_SET);//this is from beginning
            while(read(cd,&c,sizeof(c))>0)
            {
                if(c.c_id==en.cid)
                {
                    bzero(wbuffer,sizeof(wbuffer));
                    sprintf(wbuffer,"Course Details:\n\tCourse ID: %d\n\tName: %s\n\tCredits: %d\n\tSeats: %d\n\tAvailable seats:%d", c.c_id, c.name, c.credit,c.seats,c.avail_seats);
                    strcat(wbuffer,"\n\n...^");
                    wb=write(connectfd, wbuffer, strlen(wbuffer));//Send course information to the client
                    if (wb == -1) 
                    {
                        perror("Error writing course information to the client");
                        return;
                    }
                    bzero(rbuffer,sizeof(rbuffer));
                    rb=read(connectfd,rbuffer,sizeof(rbuffer)); // Dummy read
                    break;
                }
            }
        }
    }
    close(fd);
    close(cd);
}


bool student_operation_handler(int connectfd)
{
    int id=login_student(connectfd);
    if (id!=-1)//if login successfull then we have to do this
    {
        ssize_t wbytes,rbytes;            
        char rbuffer[1000], wbuffer[1000];
        bzero(wbuffer,sizeof(wbuffer));
        bzero(rbuffer,sizeof(rbuffer));
        while (1)
        {
            strcat(wbuffer,"\n");
            strcat(wbuffer,STUDENT_MENU);
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
                view_all_courses(connectfd);
                break;
                case 2:
                enroll_courses(connectfd,id);
                break;
                case 3:
                drop_course(connectfd,id);
                break;
                case 4:
                view_enroll_course(connectfd,id);
                break;
                case 5:
                change_student_password(connectfd,id);
                break;
                case 6:
                student_logout_exit(connectfd);
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
