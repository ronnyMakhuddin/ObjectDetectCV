
// ObjDetectDlg.cpp : implementation file
//

#include "stdafx.h"


#include "ObjDetect.h"
#include "ObjDetectDlg.h"
#include "afxdialogex.h"
#include "HarrisDetector.h"



using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



int opencv_load_image();
int opencv_open_webcam();
int cartoonifyFrame(Mat& src, Mat& des);


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CObjDetectDlg dialog




CObjDetectDlg::CObjDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CObjDetectDlg::IDD, pParent)
	,m_bStartProcessing(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CObjDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_INPUT, m_imgInput);
	DDX_Control(pDX, IDC_PIC_OUTPUT, m_ImgOutput);
	DDX_Control(pDX, IDC_COMBO2, m_cmbFeatureDetectAlgo);
	DDX_Control(pDX, IDC_PIC_OUTPUT_GRAY, m_imgOutputGray);
	DDX_Control(pDX, IDC_CHK_FEATURE, m_chkFeatureMatching);
	DDX_Control(pDX, IDC_SLIDER1, m_slThreshold);
	DDX_Control(pDX, IDC_SLIDER2, m_slQualityLevel);
	DDX_Control(pDX, IDC_SLIDER3, m_slMinDistance);
	DDX_Control(pDX, IDC_SLIDER4, m_slFeatures);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, IDC_STOP, m_btnStop);
}

BEGIN_MESSAGE_MAP(CObjDetectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DONE, &CObjDetectDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CANCEL, &CObjDetectDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_LOAD_CAM, &CObjDetectDlg::OnBnClickedBtnLoadCam)
	ON_BN_CLICKED(IDC_BTN_LOAD_VIDEO, &CObjDetectDlg::OnBnClickedBtnLoadVideo)
	ON_BN_CLICKED(IDC_BTN_LOAD_IMG, &CObjDetectDlg::OnBnClickedBtnLoadImg)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO2, &CObjDetectDlg::OnCbnSelchangeCombo2)
	ON_BN_CLICKED(IDC_CHK_FEATURE, &CObjDetectDlg::OnBnClickedChkFeature)
	ON_BN_CLICKED(IDC_START, &CObjDetectDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CObjDetectDlg::OnBnClickedStop)
END_MESSAGE_MAP()


// CObjDetectDlg message handlers

BOOL CObjDetectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_cmbFeatureDetectAlgo.AddString(L"GOOD");
	m_cmbFeatureDetectAlgo.AddString(L"FAST");
	m_cmbFeatureDetectAlgo.AddString(L"SURF");
	m_cmbFeatureDetectAlgo.AddString(L"SIFT");

	m_cmbFeatureDetectAlgo.SetCurSel(1); //FAST
	m_bStartProcessing = false;
	m_btnStop.EnableWindow(FALSE);

	m_pDCInput = m_imgInput.GetDC();
	m_imgInput.GetClientRect(&m_rectInput);
	m_pDCOutput = m_ImgOutput.GetDC();
	m_ImgOutput.GetClientRect(&m_rectOutput);
	m_pDCOutputGray = m_imgOutputGray.GetDC();
	m_imgOutputGray.GetClientRect(&m_rectOutputGray);

	//camera initialization
	int nCamNum = 0;
	m_cvCamera.open(nCamNum);
	if(!m_cvCamera.isOpened())
	{
		cerr<< "ERROR: Could not access the camera" <<endl;
		//return -1;
	}

	//Try to set the camera resolution
	m_cvCamera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	m_cvCamera.set(CV_CAP_PROP_FRAME_HEIGHT,480);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CObjDetectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CObjDetectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CObjDetectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CObjDetectDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	this->OnOK();
}


void CObjDetectDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	this->OnCancel();
}


void CObjDetectDlg::OnBnClickedBtnLoadCam()
{
	// TODO: Add your control notification handler code here
	//opencv_open_webcam();
	SetTimer(1,30,NULL);
	

}



void CObjDetectDlg::OnBnClickedBtnLoadVideo()
{
	// TODO: Add your control notification handler code here
	SetTimer(2,30,NULL);
}

cv::Mat gnPrevFrame;
void CObjDetectDlg::CaptureVideoFileMode()
{
	cv::VideoCapture capture("cat.mp4");
	if(!capture.isOpened())
		return;

	double rate = capture.get(CV_CAP_PROP_FPS);
	bool stop(false);
	cv::Mat frame;
	int nDelay = 1000/rate;


	while(!stop)
	{
		if(!capture.read(frame))
			return;

		DrawOnPicCtrl(m_pDCOutput, frame);


		if(m_bStartProcessing)
		{
			cv::Mat grayScaleFrame;
			cvtColor(frame, grayScaleFrame, COLOR_RGB2GRAY);
			DrawOnPicCtrl(m_pDCOutputGray, grayScaleFrame);
			cv::Mat match;
			if(gnPrevFrame.data)
			{
				CHarrisDetector hDetect;
				//match = hDetect.CompareTwoImages(grayScaleFrame, gnPrevFrame);
				//DrawOnPicCtrl(m_pDCOutputGray, match);

			}
			gnPrevFrame = grayScaleFrame;
		}
	
		Sleep(10);
		char cKeyPress = cv::waitKey(10);
		if(cKeyPress == 27)
		{
			stop = true;
			break;


		}
	}

	capture.release();
	




}
void CObjDetectDlg::CaptureWebCamContinuousMode()
{
	int nCamNum = 0;

	//Get Access to camera
	cv::VideoCapture camera;
	camera.open(nCamNum);
	if(!camera.isOpened())
	{
		cerr<< "ERROR: Could not access the camera" <<endl;
		return ;
	}

	//Try to set the camera resolution
	camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT,480);

	while(true)
	{
		//Get the next frame
		cv::Mat cameraFrame;
		camera >> cameraFrame;

		if(cameraFrame.empty())
		{
			cerr << "ERROR: Could not get the camera frame."<<endl;
			return ;
		}

		DrawOnPicCtrl(m_pDCOutput, cameraFrame);
		char cKeyPress = cv::waitKey(10);
		if(cKeyPress == 27)
		{
			break;
			
		}
	}

}


void CObjDetectDlg::CaptureWebCamFrameMode()
{
	//int nCamNum = 0;

	////Get Access to camera
	//cv::VideoCapture camera;
	//camera.open(nCamNum);
	//if(!camera.isOpened())
	//{
	//	cerr<< "ERROR: Could not access the camera" <<endl;
	//	return ;
	//}

	////Try to set the camera resolution
	//camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//camera.set(CV_CAP_PROP_FRAME_HEIGHT,480);

	//Get the next frame
	cv::Mat cameraFrame;
	cv::Mat grayScaleFrame;
	m_cvCamera >> cameraFrame;

	cvtColor(cameraFrame, grayScaleFrame, COLOR_RGB2GRAY);


	if(cameraFrame.empty())
	{
		cerr << "ERROR: Could not get the camera frame."<<endl;
		return ;
	}

	DrawOnPicCtrl(m_pDCOutput, cameraFrame);

	//GetTheHarrisDetectorImage(grayScaleFrame);
	//
	if(m_bStartProcessing)
	{
		cv::Mat match;
		if(gnPrevFrame.data)
		{
			if(m_chkFeatureMatching.GetCheck()== BST_CHECKED)
			{
				CHarrisDetector hDetect;
				match = hDetect.CompareTwoImages(grayScaleFrame, gnPrevFrame);
				DrawOnPicCtrl(m_pDCOutputGray, match);
			}
			else
			{
				GetFeatureExtraction(grayScaleFrame);
				DrawOnPicCtrl(m_pDCOutputGray, grayScaleFrame);

			}

		}
		gnPrevFrame = grayScaleFrame;
	}
	
	

}

void CObjDetectDlg::GetFeatureExtraction(cv::Mat& mtInputFrame)
{
	CHarrisDetector harrisDetect;
	switch(m_nFeatureDetectAlgo)
	{
	case 0 : harrisDetect.GetGoodFeaturePoints(mtInputFrame); break;
	case 1 : harrisDetect.GetFASTFeaturePoints(mtInputFrame); break;
	case 2 : harrisDetect.GetSURFFeaturePoints(mtInputFrame); break;
	case 3:  harrisDetect.GetSIFTFeaturePoints(mtInputFrame); break;

	default:
		harrisDetect.GetFASTFeaturePoints(mtInputFrame);
	}
	
}

void CObjDetectDlg::GetTheHarrisDetectorImage(cv::Mat& mtInputFrame)
{
	//Create Harris Detect Instance
	CHarrisDetector harrisDetect;
	//Get the Harris Values
	harrisDetect.DetectHarrisCorner(mtInputFrame);
	//Detect Harris COrners
	std::vector<cv::Point> pts;
	harrisDetect.GetCorners(pts, 0.01);
	//Draw Harris COrner
	harrisDetect.DrawOnImage(mtInputFrame, pts);
}




 int CObjDetectDlg::GetBpp(cv::Mat img)
{
	return 8 * img.channels();
} 

void CObjDetectDlg::DrawOnPicCtrl(CDC* pDC, cv::Mat& mtInputFrame)
{
	int height=mtInputFrame.rows;
	int width= mtInputFrame.cols;
	uchar buffer[sizeof( BITMAPINFOHEADER ) + 1024]; 
	BITMAPINFO* bmi = (BITMAPINFO* )buffer; 
	FillBitmapInfo(bmi,width,height,GetBpp(mtInputFrame),0);
	pDC->SetStretchBltMode(COLORONCOLOR);
	/*SetDIBitsToDevice(pDC->GetSafeHdc(), 0, 0, width,
		height, 0, 0, 0, height, image.data, bmi,
		DIB_RGB_COLORS);
	*/
	::StretchDIBits(pDC->GetSafeHdc(), m_rectInput.left, m_rectInput.top, m_rectInput.right, m_rectInput.bottom,
	0, 0, width, height, mtInputFrame.data,bmi,
	 DIB_RGB_COLORS, SRCCOPY);
}

void CObjDetectDlg::FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin) 
{ 
	assert(bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32)); 

	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader); 

	memset(bmih, 0, sizeof(*bmih)); 
	bmih->biSize = sizeof(BITMAPINFOHEADER); 
	bmih->biWidth = width; 
	bmih->biHeight = origin ? abs(height) : -abs(height); 
	bmih->biPlanes = 1; 
	bmih->biBitCount = (unsigned short)bpp; 
	bmih->biCompression = BI_RGB; 

	if (bpp == 8) 
	{ 
		RGBQUAD* palette = bmi->bmiColors; 

		for (int i = 0; i < 256; i++) 
		{ 
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i; 
			palette[i].rgbReserved = 0; 
		} 
	} 
}


void CObjDetectDlg::OnBnClickedBtnLoadImg()
{
	// TODO: Add your control notification handler code here
	//opencv_load_image();
	Mat image;
    image = imread("C:\\res\\mine.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return ;
    }

	DrawOnPicCtrl(m_pDCInput, image);
    //namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
   // imshow( "Display window", image );                   // Show our image inside it.

   // waitKey(0);           

}

void CObjDetectDlg::OnCbnSelchangeCombo2()
{
	m_nFeatureDetectAlgo = m_cmbFeatureDetectAlgo.GetCurSel();
}



void CObjDetectDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnTimer(nIDEvent);

	if(nIDEvent == 1) //Open WebCAM
	{
		//opencv_open_webcam();
		CaptureWebCamFrameMode();


	}
	else if(nIDEvent == 2) //Open VIdeo File
	{
		CaptureVideoFileMode();

	}
}

//external methods for testing
int opencv_open_webcam()
{
	int nCamNum = 0;

	//Get Access to camera
	cv::VideoCapture camera;
	camera.open(nCamNum);
	if(!camera.isOpened())
	{
		cerr<< "ERROR: Could not access the camera" <<endl;
		return -1;
	}

	//Try to set the camera resolution
	camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT,480);

	while(true)
	{
		//Get the next frame
		cv::Mat cameraFrame;
		camera >> cameraFrame;

		if(cameraFrame.empty())
		{
			cerr << "ERROR: Could not get the camera frame."<<endl;
			return -1;
		}

		//create a blank output image, that we will draw into
		cv::Mat displayFrame(cameraFrame.size(), CV_8UC3);
		//modify here
		cartoonifyFrame(cameraFrame, displayFrame);
		//display processed image here
		imshow("camera", cameraFrame);
		imshow("Cartoonifier", displayFrame);
		
		
		char cKeyPress = cv::waitKey(10);
		if(cKeyPress == 27)
		{
			break;
			
		}
	}
	return 0;
}

int cartoonifyFrame(Mat& src, Mat& des)
{
		cvtColor(src, des, CV_BGR2GRAY);
		const int MEDIAN_BLUR_FILTER_SIZE = 7;
		medianBlur(des, des, MEDIAN_BLUR_FILTER_SIZE);

		Mat edges;
		const int LAPLACIAN_FILTER_SIZE = 5;
		Laplacian(des, edges,CV_8U, LAPLACIAN_FILTER_SIZE);

		Mat mask;
		const int EDGES_THRESHOLD = 10;
		threshold(edges, mask, EDGES_THRESHOLD, 255, THRESH_BINARY_INV);

		Size size = src.size();
		Size smallSize;
		smallSize.width = size.width/2;
		smallSize.height = size.height/2;

		Mat smallImg = Mat(smallSize, CV_8UC3);
		resize(src, smallImg, smallSize, 0,0, INTER_LINEAR);
		Mat tmp = Mat(smallSize, CV_8UC3);
		int repetitions = 7;
		for (int i =0; i < repetitions; i++)
		{
			int ksize = 9;
			double sigmaColor = 9;
			double sigmaSpace = 7;

			bilateralFilter(smallImg, tmp, ksize, sigmaColor, sigmaSpace);
			bilateralFilter(tmp, smallImg, ksize, sigmaColor, sigmaSpace);
		}

		Mat bigImg;
		resize(smallImg, bigImg, size, 0,0, INTER_LINEAR);
		des.setTo(0);
		bigImg.copyTo(des, mask);
	return 1;
}

int opencv_load_image()
{
	
    Mat image;
    image = imread("C:\\Res\\mine.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
	
}







void CObjDetectDlg::OnBnClickedChkFeature()
{
	// TODO: Add your control notification handler code here
}


void CObjDetectDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	m_bStartProcessing = true;
	m_btnStart.EnableWindow(FALSE);
	m_btnStop.EnableWindow(TRUE);
}


void CObjDetectDlg::OnBnClickedStop()
{
	// TODO: Add your control notification handler code here
	m_bStartProcessing = false;
	m_btnStart.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);
}
