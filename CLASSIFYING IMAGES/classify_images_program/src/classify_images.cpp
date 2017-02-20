/***************************************************************************
 *                                                                         *
 * CLASSIFY IMAGES                                               *
 *                                                                         *
 * Authors:                                                                *
 *   Edgar Alarcon <edgaroriolalarconpalma@gmail.com>                          *
 *                                                                         *
 ***************************************************************************/
/***************classify_images.cpp******************

Object marker for marking the objects to be detected  from positive samples and then creating the
description file for positive images.
*/



#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>

// for filelisting
#include <stdio.h>
#include <sys/io.h>
// for fileoutput
#include <string>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>

using namespace std;

IplImage* image=0;
IplImage* image2=0;
//int start_roi=0;
int roi_x0=0;
int roi_y0=0;
int roi_x1=0;
int roi_y1=0;
int numOfRec=0;
int startDraw = 0;
char* window_name="<SPACE>add <B>save and load next <ESC>exit";

string IntToString(int num)
{
    ostringstream myStream; //creates an ostringstream object
    myStream << num << flush;
    /*
    * outputs the number into the string stream and then flushes
    * the buffer (makes sure the output is put into the stream)
    */
    return(myStream.str()); //returns the string form of the stringstream object
};

void on_mouse(int event,int x,int y,int flag, void *param)
{
    if(event==CV_EVENT_LBUTTONDOWN)
    {
        if(!startDraw)
        {
            roi_x0=x;
            roi_y0=y;
            startDraw = 1;
        } else {
            roi_x1=x;
            roi_y1=y;
            startDraw = 0;
        }
    }
    if(event==CV_EVENT_MOUSEMOVE && startDraw)
    {

        //redraw ROI selection
        image2=cvCloneImage(image);
        cvRectangle(image2,cvPoint(roi_x0,roi_y0),cvPoint(x,y),CV_RGB(255,0,255),1);
        cvShowImage(window_name,image2);
        cvReleaseImage(&image2);
    }

}

int main()
{
    char iKey=0;
    string strPrefix;
    string strPostfix;
    string input_directory;
    string output_directory;

    input_directory ="../not_classified_images";
    output_directory="..";

    /* Get a file listing of all files with in the input directory */
    DIR    *dir_p = opendir (input_directory.c_str());
    struct dirent *dir_entry_p;

    if(dir_p == NULL) {
        fprintf(stderr, "Failed to open directory %s\n", input_directory.c_str());
        return -1;
    }

    cvAddSearchPath(input_directory);
    cvNamedWindow(window_name,1);
    cvSetMouseCallback(window_name,on_mouse, NULL);

    int num_imagen=0;
    fprintf(stderr, "For each image:  \n <Space>    add rectangle to current image \n <ENTER>    save added rectangles and show next image\n  <z>   send image to negative images folder\n <ESC>   exit program\n");

    while((dir_entry_p = readdir(dir_p)) != NULL)
    {
        numOfRec=0;


        if(strcmp(dir_entry_p->d_name, ""))
        fprintf(stderr, "Examining file %s\n", dir_entry_p->d_name);


        strPostfix="";
        strPrefix=dir_entry_p->d_name;
        fprintf(stderr, "Loading image %s\n", strPrefix.c_str());
        image=cvLoadImage((input_directory+"/"+strPrefix).c_str(),1);

        if(image!=0)

        {
            num_imagen++;
            fprintf(stderr, "IMAGEN %d\n", num_imagen);

            //    work on current image
            do

            {
                cvShowImage(window_name,image);

                // used cvWaitKey returns:
                //    <ENTER>=13        save added rectangles and show next image
                //    <ESC>=27        exit program
                //    <Space>=32        add rectangle to current image
                //    <z>=122       send image to negative images folder
                //  any other key clears rectangle drawing only
                iKey=cvWaitKey(0);
                switch(iKey)
                {

                case 27:

                        cvReleaseImage(&image);
                        cvDestroyWindow(window_name);
                        return 0;
                case 32:

                        numOfRec++;
                printf("   %d. rect x=%d\ty=%d\tx2h=%d\ty2=%d\n",numOfRec,roi_x0,roi_y0,roi_x1,roi_y1);
                //printf("   %d. rect x=%d\ty=%d\twidth=%d\theight=%d\n",numOfRec,roi_x1,roi_y1,roi_x0-roi_x1,roi_y0-roi_y1);
                        // currently two draw directions possible:
                        //        from top left to bottom right or vice versa
                        if(roi_x0<roi_x1 && roi_y0<roi_y1)
                        {

                            printf("   %d. rect x=%d\ty=%d\twidth=%d\theight=%d\n",numOfRec,roi_x0,roi_y0,roi_x1-roi_x0,roi_y1-roi_y0);
                            // append rectangle coord to previous line content
                            strPostfix+=" "+IntToString(roi_x0)+" "+IntToString(roi_y0)+" "+IntToString(roi_x1-roi_x0)+" "+IntToString(roi_y1-roi_y0);

                        }


                        break;
                }
            }
            while(iKey!=13 && iKey!=122);


            // save to info file as later used for HaarTraining:
            //    <rel_path>\bmp_file.name numOfRec x0 y0 width0 height0 x1 y1 width1 height1...
            if(numOfRec>0 && iKey==13)
            {

                cvSetImageROI(image,cvRect(roi_x0,roi_y0,roi_x1-roi_x0,roi_y1-roi_y0));
                IplImage *cropped = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
                cvCopy(image, cropped, NULL);
                cvResetImageROI(image);
                cvSaveImage((output_directory+"/cropped_positive_images/cropped_"+strPrefix).c_str(),cropped);
                rename ((input_directory+"/"+strPrefix).c_str(), (output_directory+"/positive_images/"+strPrefix).c_str());
                cvReleaseImage(&image);
            }


            if (iKey==122){
                rename ((input_directory+"/"+strPrefix).c_str(), (output_directory+"/negative_images/"+strPrefix).c_str());
                cvReleaseImage(&image);
            }
    }
//    else fprintf(stderr,"NOT LOADED CORRECTLY\n");
    }


    cvDestroyWindow(window_name);
    closedir(dir_p);

    return 0;
}
