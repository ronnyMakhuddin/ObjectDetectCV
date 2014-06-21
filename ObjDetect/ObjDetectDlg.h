
// ObjDetectDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "afxcmn.h"


// CObjDetectDlg dialog
class CObjDetectDlg : public CDialogEx
{
// Construction
public:
	CObjDetectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OBJDETECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin);
	void DrawOnPicCtrl( CDC* pDC, cv::Mat& mtInputFrame);
	int GetBpp(cv::Mat img);
	void CaptureWebCamContinuousMode();
	void CaptureWebCamFrameMode();
	void CaptureVideoFileMode();
	void GetTheHarrisDetectorImage(cv::Mat& mtInputFrame);
	void GetFeatureExtraction(cv::Mat& mtInputFrame);
	


// Implementation
protected:
	HICON m_hIcon;
	CRect m_rectInput;
	CRect m_rectOutput;
	CRect m_rectOutputGray;
	CDC   *m_pDCInput;
	CDC   *m_pDCOutput;
	CDC   *m_pDCOutputGray;
	cv::VideoCapture m_cvCamera;
	int m_nFeatureDetectAlgo;
	bool m_bStartProcessing;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnLoadCam();
	afx_msg void OnBnClickedBtnLoadVideo();
	afx_msg void OnBnClickedBtnLoadImg();
	CStatic m_imgInput;
	CStatic m_ImgOutput;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_cmbFeatureDetectAlgo;
	afx_msg void OnCbnSelchangeCombo2();
	CStatic m_imgOutputGray;
	CButton m_chkFeatureMatching;
	afx_msg void OnBnClickedChkFeature();
	CSliderCtrl m_slThreshold;
	CSliderCtrl m_slQualityLevel;
	CSliderCtrl m_slMinDistance;
	CSliderCtrl m_slFeatures;
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	CButton m_btnStart;
	CButton m_btnStop;
};
