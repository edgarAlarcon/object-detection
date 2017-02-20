/***************************************************************************
 *                                                                         *
 * YouBotArm Demo Programm                                                 *
 *                                                                         *
 * Authors:                                                                *
 *   Johannes Michael <johannes.michael@unibw.de>                          *
 *                                                                         *
 ***************************************************************************/
/***************objectmarker.cpp******************

Objectmarker for marking the objects to be detected  from positive samples and then creating the
description file for positive images.

compile this code and run with two arguments, first one the name of the descriptor file and the second one
the address of the directory in which the positive images are located

while running this code, each image in the given directory will open up. Now mark the edges of the object using the mouse buttons
  then press then press "SPACE" to save the selected region, or any other key to discard it. Then use "B" to move to next image. the program automatically
  quits at the end. press ESC at anytime to quit.

  *the key B was chosen  to move to the next image because it is closer to SPACE key and nothing else.....

author: achu_wilson@rediffmail.com
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
    string output_file;
    string output_directory;
    string output_file2;
/*
    if(argc != 3) {
        fprintf(stderr, "%s output_info.txt raw/data/directory/\n", argv[0]);
        return -1;
    }

    input_directory = argv[2];
    output_file = argv[1];*/

    output_file ="../positive_images/positives.txt";
    output_file2 ="../positive_images/cropped_positives.txt";
    input_directory ="/home/edgar/HAAR_TRAINING/positive_images/images";
    output_directory="/home/edgar/HAAR_TRAINING/positive_images/cropped";
    /* Get a file listing of all files with in the input directory */
    DIR    *dir_p = opendir (input_directory.c_str());
    struct dirent *dir_entry_p;

    if(dir_p == NULL) {
        fprintf(stderr, "Failed to open directory %s\n", input_directory.c_str());
        return -1;
    }

    fprintf(stderr, "Object Marker: Input Directory: %s  Output File: %s\n", input_directory.c_str(), output_file.c_str());


    cvAddSearchPath(input_directory);
    cvNamedWindow(window_name,1);
    cvSetMouseCallback(window_name,on_mouse, NULL);

    fprintf(stderr, "Opening directory...");

    ofstream output(output_file.c_str());
    ofstream output2(output_file2.c_str());
    fprintf(stderr, "done.\n");
    int num_imagen=0;
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
                        output.close();
                        output2.close();
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

                output << ("/images/"+strPrefix).c_str() << " "<< numOfRec << strPostfix <<"\n";
                output2 << ("/cropped/cropped_"+strPrefix).c_str()<<"\n";

                cvSetImageROI(image,cvRect(roi_x0,roi_y0,roi_x1-roi_x0,roi_y1-roi_y0));
                IplImage *cropped = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
                cvCopy(image, cropped, NULL);
                cvResetImageROI(image);
                cvSaveImage((output_directory+"/cropped_"+strPrefix).c_str(),cropped);
                cvReleaseImage(&image);
            }


            if (iKey==122){
                rename ((input_directory+"/"+strPrefix).c_str(), (input_directory+"/negatives/"+strPrefix).c_str());
                cvReleaseImage(&image);
            }
    }
    else fprintf(stderr,"NOT LOADED CORRECTLY\n");
    }

    output.close();
    output2.close();
    cvDestroyWindow(window_name);
    closedir(dir_p);

    return 0;
}
