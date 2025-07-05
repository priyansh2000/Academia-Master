#ifndef COURSES
#define COURSES
struct courses
{
    int c_id;//this is for course id
    char name[50];//this is for course name
    int f_id;//this is for faculty id who is offering -1 means no is offering
    int credit;//this is for credits
    int seats;//this is for number of students who can take seats only particular students only can take it 
    int avail_seats;//this will tell how many available seats
};
#endif