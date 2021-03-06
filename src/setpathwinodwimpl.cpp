#include "setpathwinodwimpl.h"
#include <QtGui>
#include "item.h"
#include <iostream>
#include "tangible_type.h"

SetPathWindowImpl::SetPathWindowImpl( QGraphicsItem *item,QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
	{
		setupUi(this);
		setAttribute(Qt::WA_DeleteOnClose);
		count = 0;
		scene = new SetPathScene;
		setpathitem = dynamic_cast<Tangible_Type*>(item);
		
		pathset = 0 ;
        
		
		QPen pen;
		pen.setBrush(Qt::blue);
		QGraphicsRectItem *rect =  scene->addRect(5,5,600,400,pen);
		rect->setFlag(QGraphicsItem::ItemIsSelectable,false);
    	view->setScene(scene);
    	view->setRenderHints(QPainter::Antialiasing
                         | QPainter::TextAntialiasing);
                         
        smoothness->setMinimum(0.05);
        smoothness->setMaximum(0.95);
        smoothness->setValue(0.75);
        pointx->setMinimum(0);
        pointy->setMinimum(0);
        pointx->setMaximum(800);
        pointy->setMaximum(800);
        
        scene->x.insert(0,0);
        scene->y.insert(0,0);
        scene->x.insert(1,0);
        scene->y.insert(1,0);
		connect(addPointButton,SIGNAL(clicked()),this,SLOT(addPoint()));
		connect(previewPathButton,SIGNAL(clicked()),this,SLOT(previewPath()));
		connect(scene,SIGNAL(addPoint(double,double)),this,SLOT(addPointOfScene(double,double)));
		connect(setPathButton,SIGNAL(clicked()),this,SLOT(setpath()));
		connect(this,SIGNAL(setPathSignal()),this,SLOT(previewPath()));
		startTimeBox->setMinimum(0);
		startTimeBox->setMaximum(1);
		stopTimeBox->setMinimum(0);
		stopTimeBox->setMaximum(1);
		startTimeBox->setValue(0);
		stopTimeBox->setValue(1);
		path = new QPainterPath;

	}

void SetPathWindowImpl::addPoint()
{
	if (index->value() > count) count = index->value();
	scene->x.insert(index->value(),pointx->value());
	scene->y.insert(index->value(),pointy->value());	
	//std::cout << index->value() << "   \n" ;
	index->setValue(index->value() + 1);
	scene->addEllipse(pointx->value()-2,pointy->value()-2,5,5);
	 
}


void SetPathWindowImpl::addPointOfScene(double i,double j)
{
	if (index->value() > count) count = index->value();
	scene->x.insert(index->value(),i);
	scene->y.insert(index->value(),j);	
	//std::cout << index->value() << scene->x.at(index->value())<<scene->y.at(index->value())<<"   \n" ;
	index->setValue(index->value() + 1);
	scene->addEllipse(i-2,j-2,5,5);
	 
}

void SetPathWindowImpl::previewPath()
{	
	double cp1x;
	double cp2x;
	double cp1y;
	double cp2y;


         
         

	
	path->moveTo(scene->x.at(0),scene->y.at(0));
	QPen path_pen ;
	
	int i = 1;
	while ( i > 0 && i < (count))
	{
		
		bezierControlPoint(i,cp1x,cp1y,cp2x,cp2y);
		path->cubicTo(cp1x,cp1y,cp2x,cp2y,scene->x.at(i+1),scene->y.at(i+1));
		//std::cout<< "Generating Control Point for " << i << scene->x.at(i+1)<<scene->y.at(i+1)<<"   \n";
		//std::cout<< cp1x << "   " << cp1y << "   \n";
		//std::cout<< cp2x << "   " << cp2y << "    \n";
		i ++ ;
		
		
	}
	

	//std::cout << "Drawing Path" <<"  \n";
	path_pen.setBrush(Qt::red);
	path_pen.setWidth(3);
	
	scene->addPath(*path,path_pen);
	pathset = 1;
	
	
		
}

void SetPathWindowImpl::bezierControlPoint (int i , double& cp1x, double& cp1y,double& cp2x, double& cp2y)
{
	double x0 = (scene->x.at(i-1));
	double y0 = (scene->y.at(i-1));
	
	double x1 = (scene->x.at(i));
	double y1 = (scene->y.at(i));
	
	double x2 = (scene->x.at(i+1));
	double y2 = (scene->y.at(i+1));
	
	double x3 = (scene->x.at(i+2));
	double y3 = (scene->y.at(i+2));
	
	
	double xc1 = (x0 + x1) / 2.0;
    double yc1 = (y0 + y1) / 2.0;
    double xc2 = (x1 + x2) / 2.0;
    double yc2 = (y1 + y2) / 2.0;
    double xc3 = (x2 + x3) / 2.0;
    double yc3 = (y2 + y3) / 2.0;

    double len1 = sqrt((x1-x0) * (x1-x0) + (y1-y0) * (y1-y0));
    double len2 = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
    double len3 = sqrt((x3-x2) * (x3-x2) + (y3-y2) * (y3-y2));

    double k1 = len1 / (len1 + len2);
    double k2 = len2 / (len2 + len3);

    double xm1 = xc1 + (xc2 - xc1) * k1;
    double ym1 = yc1 + (yc2 - yc1) * k1;

    double xm2 = xc2 + (xc3 - xc2) * k2;
    double ym2 = yc2 + (yc3 - yc2) * k2;

    // Resulting control points. Here smoothness is mentioned
    // above coefficient K whose value should be in range [0...1].
    cp1x = xm1 + (xc2 - xm1) * (smoothness->value()) + x1 - xm1;
    cp1y = ym1 + (yc2 - ym1) * (smoothness->value()) + y1 - ym1;

    cp2x = xm2 + (xc2 - xm2) * (smoothness->value()) + x2 - xm2;
    cp2y = ym2 + (yc2 - ym2) * (smoothness->value()) + y2 - ym2;
}

void SetPathWindowImpl::setpath()
{
	if (stopTimeBox->value() <= startTimeBox->value())
	{
		
        QMessageBox::warning(this, tr("QMTSim"),
                           tr("Start and Stop time are invalid.\n"
                               "Start and Stop time is a normalized value between 0 and 1.\n "
                               "Please correct it and try again"));
                               return;
	}
	
	if(pathset == 0 ) 
		{
			emit setPathSignal();
			std::cout<<"Path not previewed"<<"\n";
		}

	double m = 1/(500.0 * (stopTimeBox->value() - startTimeBox->value()));
	double j ;
	double k = 0 ;		
    for (int i = 0; i < 500; ++i)
        {	
        	j  = i/500.0 ;	
			if ((j >= startTimeBox->value()) && (j < stopTimeBox->value()) )
			{
			
			setpathitem->animation->setPosAt(j, path->pointAtPercent(k));
         	setpathitem->path_x.replace(i,(path->pointAtPercent(k).x()));
         	setpathitem->path_y.replace(i,(path->pointAtPercent(k).y()));
         	k =   k + m;
         	//std::cout << "path_x size  "<<setpathitem->path_y.size() << "\n" ;
			}
			else if ( j <= startTimeBox->value() )
			{
			setpathitem->animation->setPosAt(j, path->pointAtPercent(0)); // set to initial point
         	setpathitem->path_x.replace(i,(path->pointAtPercent(0).x()));
         	setpathitem->path_y.replace(i,(path->pointAtPercent(0).y()));
			}
			
			else if ( j >= stopTimeBox->value() )
			{
			setpathitem->animation->setPosAt(j, path->pointAtPercent(1)); // set to final point
         	setpathitem->path_x.replace(i,(path->pointAtPercent(1).x()));
         	setpathitem->path_y.replace(i,(path->pointAtPercent(1).y()));
			}
			
			
        }
        std::cout << "path_x size  "<<setpathitem->path_y.size() << "\n" ;
		this->close();
}

