/*******************************************************************************************************
ReadModules are plugins for nomacs developed at CVL/TU Wien for the EU project READ. 

Copyright (C) 2016 Markus Diem <diem@caa.tuwien.ac.at>
Copyright (C) 2016 Stefan Fiel <fiel@caa.tuwien.ac.at>
Copyright (C) 2016 Florian Kleber <kleber@caa.tuwien.ac.at>

This file is part of ReadModules.

ReadFramework is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ReadFramework is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

The READ project  has  received  funding  from  the European  Union�s  Horizon  2020  
research  and innovation programme under grant agreement No 674943

related links:
[1] http://www.caa.tuwien.ac.at/cvl/
[2] https://transkribus.eu/Transkribus/
[3] https://github.com/TUWien/
[4] http://nomacs.org
*******************************************************************************************************/

#include "Forms.h"

// nomacs
#include "DkImageStorage.h"
#include "DkSettings.h"

#include "Settings.h"
#include "Image.h"
#include "FormAnalysis.h"
#include "LineTrace.h"
#include "Algorithms.h"
#include "PageParser.h"
#include "Elements.h"

#pragma warning(push, 0)	// no warnings from includes - begin
#include <QAction>
#include <QSettings>
#pragma warning(pop)		// no warnings from includes - end

namespace rdm {

/**
*	Constructor
**/
FormsAnalysis::FormsAnalysis(QObject* parent) : QObject(parent) {

	// create run IDs
	QVector<QString> runIds;
	runIds.resize(id_end);
	
	runIds[id_train] = "871fe4d1de79497388292ef534ff25d8";
	runIds[id_show] = "d3359f4a4ec943e3abd4860e68152769";
	runIds[id_classify] = "91fd4f094fc34085a7a8715142f2292d";
	runIds[id_classifyxml] = "5f34992f7494400d9beb298e0800a571";
	mRunIDs = runIds.toList();

	// create menu actions
	QVector<QString> menuNames;
	menuNames.resize(id_end);

	menuNames[id_train] = tr("Train forms");
	menuNames[id_show] = tr("Shows form information");
	menuNames[id_classify] = tr("Apply Template");
	menuNames[id_classifyxml] = tr("Classify based on xml separators");
	mMenuNames = menuNames.toList();

	// create menu status tips
	QVector<QString> statusTips;
	statusTips.resize(id_end);

	statusTips[id_train] = tr("train forms");
	statusTips[id_show] = tr("Show");
	statusTips[id_classify] = tr("Apply Template");
	statusTips[id_classifyxml] = tr("Classify based on xml separators");
	mMenuStatusTips = statusTips.toList();

	loadSettings(nmc::DkSettingsManager::instance().qSettings());
}
/**
*	Destructor
**/
FormsAnalysis::~FormsAnalysis() {

	qDebug() << "destroying forms plugin...";

}


/**
* Returns unique ID for the generated dll
**/
QString FormsAnalysis::id() const {

	return PLUGIN_ID;
};

/**
* Returns descriptive iamge for every ID
* @param plugin ID
**/
QImage FormsAnalysis::image() const {

	return QImage(":/ReadConfig/img/read.png");
}
QString FormsAnalysis::name() const {
	return "Form Analysis";
}

QList<QAction*> FormsAnalysis::createActions(QWidget* parent) {

	if (mActions.empty()) {

		for (int idx = 0; idx < id_end; idx++) {
			QAction* ca = new QAction(mMenuNames[idx], parent);
			ca->setObjectName(mMenuNames[idx]);
			ca->setStatusTip(mMenuStatusTips[idx]);
			ca->setData(mRunIDs[idx]);	// runID needed for calling function runPlugin()
			mActions.append(ca);
		}
	}

	return mActions;
}



QList<QAction*> FormsAnalysis::pluginActions() const {
	return mActions;
}

/**
* Main function: runs plugin based on its ID
* @param plugin ID
* @param image to be processed
**/
QSharedPointer<nmc::DkImageContainer> FormsAnalysis::runPlugin(
	const QString &runID,
	QSharedPointer<nmc::DkImageContainer> imgC,
	const nmc::DkSaveInfo& saveInfo,
	QSharedPointer<nmc::DkBatchInfo>& info) const {

	if (!imgC)
		return imgC;

	if(runID == mRunIDs[id_train]) {

		QImage img = imgC->image();
		////imgC->setImage(img.mirrored(), "Mirrored");


		QSharedPointer<FormsInfo> testInfo(new FormsInfo(runID, imgC->filePath()));
		//QString loadXmlPath = rdf::PageXmlParser::imagePathToXmlPath(imgC->filePath());
		////QString saveXmlPath = rdf::PageXmlParser::imagePathToXmlPath(imgC->filePath());

		////just save xml path of template to FormsInfo
		//testInfo->setXMLTemplate(loadXmlPath);
		////mLineTemplPath = loadXmlPath;

		//rdf::PageXmlParser parser;
		//parser.read(loadXmlPath);
		//auto pe = parser.page();

		////read xml separators and store them to testinfo
		//QVector<rdf::Line> hLines;
		//QVector<rdf::Line> vLines;

		//QVector<QSharedPointer<rdf::Region>> test = rdf::Region::allRegions(pe->rootRegion());// pe->rootRegion()->children();
		//																					  //QVector<rdf::TableCell> cells;
		//QVector<QSharedPointer<rdf::TableCell>> cells;
		//QSharedPointer<rdf::TableRegion> region;


		//for (auto i : test) {

		//	if (i->type() == i->type_table_region) {
		//		region = i.dynamicCast<rdf::TableRegion>();

		//	} else if (i->type() == i->type_table_cell) {
		//		//rdf::TableCell* tCell = dynamic_cast<rdf::TableCell*>(i.data());
		//		QSharedPointer<rdf::TableCell> tCell = i.dynamicCast<rdf::TableCell>();
		//		cells.push_back(tCell);

		//		//check if tCell has a Textline as child, if yes, mark as table header;
		//		if (!tCell->children().empty()) {
		//			QVector<QSharedPointer<rdf::Region>> childs = tCell->children();
		//			for (auto child : childs) {
		//				if (child->type() == child->type_text_line) {
		//					tCell->setHeader(true);
		//					//qDebug() << imgC->filePath() << "detected header...";
		//					qDebug() << "detected header...";
		//					break;
		//				}
		//			}
		//		}


		//		if (tCell && tCell->header()) {

		//			if (tCell->topBorderVisible()) {
		//				hLines.push_back(tCell->topBorder());
		//			}
		//			if (tCell->bottomBorderVisible()) {
		//				hLines.push_back(tCell->bottomBorder());
		//			}
		//			if (tCell->leftBorderVisible()) {
		//				vLines.push_back(tCell->leftBorder());
		//			}
		//			if (tCell->rightBorderVisible()) {
		//				vLines.push_back(tCell->rightBorder());
		//			}
		//		}

		//	}
		//}

		//std::sort(cells.begin(), cells.end());
		
		
		testInfo->setFormName(imgC->filePath());
		testInfo->setFormSize(img.size());
		//testInfo->setLines(hLines, vLines);
		//testInfo->setCells(cells);
		//testInfo->setRegion(region);
			
		qDebug() << "template calculated...";

		info = testInfo;
	}
	else if(runID == mRunIDs[id_show]) {

		QImage img = imgC->image();
		cv::Mat imgIn = rdf::Image::qImage2Mat(img);
		cv::Mat imgInG;
		if (imgIn.channels() != 1) cv::cvtColor(imgIn, imgInG, CV_RGB2GRAY);

		qDebug() << imgC->filePath() << "reading XML...";
		
		QString loadXmlPath = rdf::PageXmlParser::imagePathToXmlPath(imgC->filePath());
		//QString saveXmlPath = rdf::PageXmlParser::imagePathToXmlPath(imgC->filePath());

		rdf::PageXmlParser parser;
		parser.read(loadXmlPath);
		auto pe = parser.page();

		//read xml separators and store them to testinfo
		QVector<rdf::Line> hLines;
		QVector<rdf::Line> vLines;


		QVector<QSharedPointer<rdf::Region>> test = rdf::Region::allRegions(pe->rootRegion());// pe->rootRegion()->children();
		//QVector<rdf::TableCell> cells;
		QVector<QSharedPointer<rdf::TableCell>> cells;


		for (auto i : test) {
			if (i->type() == i->type_table_cell) {
				//rdf::TableCell* tCell = dynamic_cast<rdf::TableCell*>(i.data());
				QSharedPointer<rdf::TableCell> tCell = i.dynamicCast<rdf::TableCell>();
				cells.push_back(tCell);

				//check if tCell has a Textline as child, if yes, mark as table header;
				if (!tCell->children().empty()) {
					QVector<QSharedPointer<rdf::Region>> childs = tCell->children();
					for (auto child : childs) {
						if (child->type() == child->type_text_line) {
							tCell->setHeader(true);
							//qDebug() << imgC->filePath() << "detected header...";
							qDebug() << "detected header...";
							break;
						}
					}
				}


				if (tCell) {
					
					if (tCell->topBorderVisible()) {
						hLines.push_back(tCell->topBorder());
					}
					if (tCell->bottomBorderVisible()) {
						hLines.push_back(tCell->bottomBorder());
					}
					if (tCell->leftBorderVisible()) {
						vLines.push_back(tCell->leftBorder());
					}
					if (tCell->rightBorderVisible()) {
						vLines.push_back(tCell->rightBorder());
					}
				}

			}
		}

		std::sort(cells.begin(), cells.end());

		QImage result = rdf::Image::mat2QImage(imgIn);

		QPainter myPainter(&result);
		myPainter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap));
		//myPainter.drawLine(QPoint(0,0), QPoint(500,500));

		for (int i = 0; i < hLines.size(); i++) {
			rdf::Line lineTmp = hLines[i];
			myPainter.drawLine(lineTmp.p1().toQPoint(), lineTmp.p2().toQPoint());
			//qDebug() << "Point 1: " << lineTmp.line().p1().toQPoint() << " Point 2: " << lineTmp.line().p2().toQPoint();
		}
		myPainter.setPen(QPen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap));
		for (int i = 0; i < vLines.size(); i++) {
			rdf::Line lineTmp = vLines[i];
			myPainter.drawLine(lineTmp.p1().toQPoint(), lineTmp.p2().toQPoint());
			//qDebug() << "Point 1: " << lineTmp.line().p1().toQPoint() << " Point 2: " << lineTmp.line().p2().toQPoint();
		}


		myPainter.end();
		
		qDebug() << "Drawing form...";
		imgC->setImage(result, "Form Image");

		////test LSD here
		//// -------------- LSD -------------------------------------------
		//QImage img = imgC->image();
		//cv::Mat imgIn = rdf::Image::qImage2Mat(img);
		//cv::Mat imgInG;
		//if (imgIn.channels() != 1) cv::cvtColor(imgIn, imgInG, CV_RGB2GRAY);

		//rdf::ReadLSD lsd(imgInG);
		////ReadLSD(inputG, mask);

		//
		//lsd.compute();
		//QVector<rdf::LineSegment> detLines = lsd.lines();
		//
		//cv::Mat outImg = lsd.magImg();
		////outImg = lsd.radImg();
		//outImg = lsd.regImg();
		////cv::normalize(outImg, outImg, 255, 0, cv::NORM_MINMAX);
		////cv::Mat outImg = imgIn.clone();



		//if (outImg.channels() == 1) {
		//	outImg.convertTo(outImg, CV_32F);
		//	cv::normalize(outImg, outImg, 1, 0, cv::NORM_MINMAX);
		//	rdf::Image::imageInfo(outImg, "outimg");
		//	cv::cvtColor(outImg, outImg, CV_GRAY2BGRA);
		//}
		//	
		//QImage result = rdf::Image::mat2QImage(outImg);		

		//QPainter myPainter(&result);
		//myPainter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap));
		////myPainter.drawLine(QPoint(0,0), QPoint(500,500));

		//qDebug() << "LSD lines detected: " << detLines.size();;	

		//for (int i = 0; i < detLines.size(); i++) {

		//	rdf::LineSegment lineTmp = detLines[i];
		//	myPainter.drawLine(lineTmp.line().p1().toQPoint(), lineTmp.line().p2().toQPoint());
		//	//qDebug() << "Point 1: " << lineTmp.line().p1().toQPoint() << " Point 2: " << lineTmp.line().p2().toQPoint();
		//}
		//myPainter.end();
		//
		//qDebug() << "LSD calculated...";
		//imgC->setImage(result, "LSD Image");

		//// -------------- LSD -------------------------------------------

		////only test version
		////not yet implemented
		//QImage img = imgC->image();
		//img = img.convertToFormat(QImage::Format_Grayscale8);
		//imgC->setImage(img, "Grayscale");

		//QSharedPointer<FormsInfo> testInfo(new FormsInfo(runID, imgC->filePath()));
		//testInfo->setFormName(imgC->filePath());
		//qDebug() << "id_show... (not implemented, shows only grayscale img)";

		//info = testInfo;
	}
	else if (runID == mRunIDs[id_classify]) {
		QImage img = imgC->image();
		//imgC->setImage(img.mirrored(), "Mirrored");

		QSharedPointer<FormsInfo> testInfo(new FormsInfo(runID, imgC->filePath()));

		QString loadXmlPath = rdf::PageXmlParser::imagePathToXmlPath(imgC->filePath());
		//QString saveXmlPath = rdf::PageXmlParser::imagePathToXmlPath(imgC->filePath());

		rdf::PageXmlParser parser;
		parser.read(loadXmlPath);
		auto pe = parser.page();

		cv::Mat imgForm = rdf::Image::qImage2Mat(img);
		cv::Mat imgFormG;
		if (imgForm.channels() != 1) cv::cvtColor(imgForm, imgFormG, CV_RGB2GRAY);
		//cv::Mat maskTempl = rdf::Algorithms::estimateMask(imgTemplG);
		rdf::FormFeatures formF(imgFormG);
		
		if (!formF.compute()) {
			qWarning() << "could not compute form template " << imgC->filePath();
		}

		testInfo->setFormName(imgC->filePath());
		testInfo->setFormSize(img.size());
		testInfo->setLines(formF.horLines(), formF.verLines());
		cv::Mat tmpBinImg = formF.binaryImage();
		testInfo->setLineImg(tmpBinImg);

		qDebug() << "Form img calculated...";

		info = testInfo;


	}
	else if (runID == mRunIDs[id_classifyxml]) {


		qDebug() << "classify based on xml separators...";
		
		QSharedPointer<FormsInfo> testInfo(new FormsInfo(runID, imgC->filePath()));

		QString loadXmlPath = rdf::PageXmlParser::imagePathToXmlPath(imgC->filePath());
		//QString saveXmlPath = rdf::PageXmlParser::imagePathToXmlPath(imgC->filePath());

		rdf::PageXmlParser parser;
		parser.read(loadXmlPath);
		auto pe = parser.page();

		//read xml separators and store them to testinfo
		QVector<rdf::Line> hLines;
		QVector<rdf::Line> vLines;


		QVector<QSharedPointer<rdf::Region>> test = rdf::Region::allRegions(pe->rootRegion());// pe->rootRegion()->children();
		for (auto i : test) {
			if (i->type() == i->type_separator) {
				rdf::SeparatorRegion* tSep = dynamic_cast<rdf::SeparatorRegion*>(i.data());
				if (tSep) {
					if (tSep->line().isHorizontal(5.0))
						hLines.push_back(tSep->line());

					if (tSep->line().isVertical(5.0))
						vLines.push_back(tSep->line());
				}
					
			}
		}
		
		testInfo->setFormName(imgC->filePath());
		testInfo->setFormSize(pe->imageSize());
		testInfo->setLines(hLines, vLines);

		qDebug() << "separators read from xml...";


		info = testInfo;
	}

	// wrong runID? - do nothing
	//imgC->setImage(QImage(), "empty");
	return imgC;
}

void FormsAnalysis::preLoadPlugin() const {

	qDebug() << "[PRE LOADING] form classification/training";

}

void FormsAnalysis::postLoadPlugin(const QVector<QSharedPointer<nmc::DkBatchInfo>>& batchInfo) const {
	int runIdx = mRunIDs.indexOf(batchInfo.first()->id());

	if (runIdx == id_train) {

		//for (auto bi : batchInfo) {
		//	qDebug() << bi->filePath() << "computed...";
		//	FormsInfo* tInfo = dynamic_cast<FormsInfo*>(bi.data());

		//	////TODO: how to save vector formTemplates for classification
		//	//QVector<QSharedPointer<FormsInfo> > formTemplates;
		//	//formTemplates.push_back(QSharedPointer<FormsInfo>(tInfo));

		//	if (tInfo)
		//		qDebug() << "form template: " << tInfo->iDForm();

		//	QString loadXmlPath = rdf::PageXmlParser::imagePathToXmlPath(tInfo->filePath());
		//	QString saveXmlPath = rdf::PageXmlParser::imagePathToXmlPath(tInfo->filePath());

		//	rdf::PageXmlParser parser;
		//	parser.read(loadXmlPath);
		//	auto pe = parser.page();

		//	pe->setImageSize(tInfo->formSize());
		//	pe->setImageFileName(tInfo->formName());
		//	//pe->setCreator(QString("CVL"));
		//	//horizontal lines
		//	QVector<rdf::Line> tmp = tInfo->hLines();
		//	for (int i = 0; i < tmp.size(); i++) {

		//		QSharedPointer<rdf::SeparatorRegion> pSepR(new rdf::SeparatorRegion());
		//		pSepR->setLine(tmp[i].line());

		//		pe->rootRegion()->addUniqueChild(pSepR);
		//	}
		//	//vertical lines
		//	tmp = tInfo->vLines();
		//	for (int i = 0; i < tmp.size(); i++) {

		//		QSharedPointer<rdf::SeparatorRegion> pSepR(new rdf::SeparatorRegion());
		//		pSepR->setLine(tmp[i].line());

		//		pe->rootRegion()->addUniqueChild(pSepR);
		//	}
		//	//save pageXml
		//	parser.write(saveXmlPath, pe);

		//}
	}

	if (runIdx == id_classify || runIdx == id_classifyxml) {
		qDebug() << "[POST LOADING] classify";

		////old part
		////Read template data
		////rdf::FormFeatures templateLoader;
		////templateLoader.loadTemplateDatabase(mLineTemplPath);
		////QVector<rdf::FormFeatures> templates;
		////templates = templateLoader.templatesDb();
		////double minErr = std::numeric_limits<double>::max();

		//Read template data
		rdf::FormFeatures templateForm;
		
		
		QSharedPointer<FormsInfo> templateInfo(new FormsInfo());
		templateInfo->setXMLTemplate(mLineTemplPath);
		QString loadXmlPath = rdf::PageXmlParser::imagePathToXmlPath(mLineTemplPath);
		//QString loadXmlPath = templateInfo->xmlTemplate();
		//QString loadXmlPath = mLineTemplPath;

		rdf::PageXmlParser parser;
		parser.read(loadXmlPath);
		auto pe = parser.page();

		templateInfo->setFormSize(pe->imageSize());

		//read xml separators and store them to testinfo
		QVector<rdf::Line> hLines;
		QVector<rdf::Line> vLines;

		QVector<QSharedPointer<rdf::Region>> test = rdf::Region::allRegions(pe->rootRegion());// pe->rootRegion()->children();
																							  //QVector<rdf::TableCell> cells;
		QVector<QSharedPointer<rdf::TableCell>> cells;
		QSharedPointer<rdf::TableRegion> region;


		for (auto i : test) {

			if (i->type() == i->type_table_region) {
				region = i.dynamicCast<rdf::TableRegion>();

			}
			else if (i->type() == i->type_table_cell) {
				//rdf::TableCell* tCell = dynamic_cast<rdf::TableCell*>(i.data());
				QSharedPointer<rdf::TableCell> tCell = i.dynamicCast<rdf::TableCell>();
				cells.push_back(tCell);

				//check if tCell has a Textline as child, if yes, mark as table header;
				if (!tCell->children().empty()) {
					QVector<QSharedPointer<rdf::Region>> childs = tCell->children();
					for (auto child : childs) {
						if (child->type() == child->type_text_line) {
							tCell->setHeader(true);
							//qDebug() << imgC->filePath() << "detected header...";
							qDebug() << "detected header...";
							break;
						}
					}
				}

				float thickness = 4.0;
				//if (tCell && tCell->header()) {
				if (tCell) {

					if (tCell->topBorderVisible()) {
						rdf::Line tmpL = tCell->topBorder();
						tmpL.setThickness(thickness);
						hLines.push_back(tmpL);
					}
					if (tCell->bottomBorderVisible()) {
						rdf::Line tmpL = tCell->bottomBorder();
						tmpL.setThickness(thickness);
						hLines.push_back(tmpL);
					}
					if (tCell->leftBorderVisible()) {
						rdf::Line tmpL = tCell->leftBorder();
						tmpL.setThickness(thickness);
						vLines.push_back(tmpL);
					}
					if (tCell->rightBorderVisible()) {
						rdf::Line tmpL = tCell->rightBorder();
						tmpL.setThickness(thickness);
						vLines.push_back(tmpL);
					}
				}

			}
		}

		std::sort(cells.begin(), cells.end());
		templateInfo->setLines(hLines, vLines);
		templateInfo->setRegion(region);
		templateInfo->setCells(cells);

		templateForm.setFormName(mLineTemplPath);
		//templateForm.setSize()
		templateForm.setHorLines(hLines);
		templateForm.setVerLines(vLines);
		templateForm.setSize(cv::Size(templateInfo->formSize().width(), templateInfo->formSize().height()));
		
		for (auto bi : batchInfo) {
			qDebug() << bi->filePath() << "computed...";
			FormsInfo* tInfo = dynamic_cast<FormsInfo*>(bi.data());
			//bool match = false;
			//minErr = std::numeric_limits<double>::max();


			if (tInfo) {
				qDebug() << "testing form: " << tInfo->formName();
				qDebug() << "-------------";
			}

			qDebug() << "match against template form";

			cv::Mat lineImg = tInfo->lineImg();
			//lineImg = 255 - lineImg;
			lineImg.convertTo(lineImg, CV_32FC1, 1.0 / 255.0);

			cv::Point2d lU((int)region->leftUpper().x(), (int)region->leftUpper().y());
			//use 10 pixel as offset
			lU -= cv::Point2d(10, 10);
			
			cv::Size templSize = templateForm.sizeImg();
			cv::Mat tmplImg(lineImg.size(), CV_32FC1);
			tmplImg.setTo(0.0);
			

			rdf::LineTrace::generateLineImage(templateForm.horLines(), templateForm.verLines(), tmplImg, cv::Scalar(1.0), cv::Scalar(1.0), lU);

			rdf::Image::save(tmplImg, "D:\\tmp\\templateImg.png");
			rdf::Image::save(lineImg, "D:\\tmp\\lineImg.png");

			cv::Point alignment = cv::phaseCorrelate(tmplImg, lineImg);
			
			//rdf::FormFeatures currentForm;
			//currentForm.setHorLines(tInfo->hLines());
			//currentForm.setVerLines(tInfo->vLines());
			//currentForm.setSize(cv::Size(tInfo->formSize().width(), tInfo->formSize().height()));
			//currentForm.setFormName(tInfo->formName());

			//
			////TODO adapt compareWithTemplate
			//templateForm.compareWithTemplate(currentForm);

		}

		////QString outf;
		//QString outf = "D:\\tmp\\evalForms.txt";
		////QString outf = "F:\\flo\\evalSkew.txt";
		////outf = mFilePath;
		//QFile file(outf);
		//if (file.open(QIODevice::WriteOnly)) {

		//	for (auto bi : batchInfo) {
		//		FormsInfo* tInfo = dynamic_cast<FormsInfo*>(bi.data());
		//		QString tmpStr = tInfo->filePath();
		//		tmpStr += " matches with: ";
		//		tmpStr += tInfo->matchName();
		//		tmpStr += "\n";
		//		QTextStream out(&file);
		//		out << tmpStr;
		//	}

		//	file.close();
		//}

	}
	else
		qDebug() << "[POST LOADING] train/add training";

	//not tested....
	saveSettings(rdf::Config::instance().settings());
}

void FormsAnalysis::loadSettings(QSettings & settings) {
	settings.beginGroup("FormAnalysis");
	mLineTemplPath = settings.value("lineTemplPath", mLineTemplPath).toString();
	settings.endGroup();
}

void FormsAnalysis::saveSettings(QSettings & settings) const {
	settings.beginGroup("FormAnalysis");
	settings.setValue("lineTemplPath", mLineTemplPath);
	settings.endGroup();
}

// DkTestInfo --------------------------------------------------------------------
FormsInfo::FormsInfo(const QString& id, const QString & filePath) : nmc::DkBatchInfo(id, filePath) {
}

void FormsInfo::setFormName(const QString & p) {
	mProp = p;
}

QString FormsInfo::formName() const {
	return mProp;
}

void FormsInfo::setMatchName(const QString & p){
	mMatchName = p;
}

QString FormsInfo::matchName() const{
	return mMatchName;
}

void FormsInfo::setFormSize(const QSize & s){
	mS = s;
}

QSize FormsInfo::formSize() const{
	return mS;
}

void FormsInfo::setTemplId(int id) {
	mIdForm = id;
}

int FormsInfo::iDForm() const {
	return mIdForm;
}

void FormsInfo::setXMLTemplate(QString t) {
	mXMLTemplate = t;
}

QString FormsInfo::xmlTemplate() const {
	return mXMLTemplate;
}

void FormsInfo::setLineImg(cv::Mat & img) {
	mLineImg = img;
}

cv::Mat FormsInfo::lineImg() const {
	return mLineImg;
}

void FormsInfo::setLines(QVector<rdf::Line> hL, QVector<rdf::Line> vL) {
	mHorLines = hL;
	mVerLines = vL;
}

QVector<rdf::Line> FormsInfo::hLines() {
	return mHorLines;
}

QVector<rdf::Line> FormsInfo::vLines() {
	return mVerLines;
}

void FormsInfo::addCell(QSharedPointer<rdf::TableCell> c) {
	mCells.push_back(c);
}

void FormsInfo::setCells(QVector<QSharedPointer<rdf::TableCell>> c) {
	mCells = c;
}

QVector<QSharedPointer<rdf::TableCell>> FormsInfo::cells() const {
	return mCells;
}

void FormsInfo::setRegion(QSharedPointer<rdf::TableRegion> r) {
	mRegion = r;
}

QSharedPointer<rdf::TableRegion> FormsInfo::region() {
	return mRegion;
}

};

