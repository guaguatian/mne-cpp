//=============================================================================================================
/**
* @file     loadfilter.cpp
* @author   Lorenz Esch <lorenz.esch@tu-ilmenau.de>;
*           Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>;
* @version  1.0
* @date     April, 2015
*
* @section  LICENSE
*
* Copyright (C) 2015, Lorenz Esch, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Implementation of the LoadFilter class
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "loadfilter.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace UTILSLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

LoadFilter::LoadFilter()
{
}


//*************************************************************************************************************

bool LoadFilter::readFilter(QString path, RowVectorXd &coefficients, QString &type, QString &name, int &order, double &sFreq)
{
    //Open .txt file
    if(!path.contains(".txt"))
        return false;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Error opening filter txt file for reading";
        return false;
    }

    //Start reading from file
    QTextStream in(&file);
    QVector<double> coefficientsTemp;

    while(!in.atEnd())
    {
        QString line = in.readLine();

        QStringList fields = line.split(QRegExp("\\s+"));

        //Delete last element if it is a blank character
        if(fields.at(fields.size()-1) == "")
            fields.removeLast();

        if(line.contains("#")) //Filter meta information commented areas in file
        {
            //Read filter order
            if(line.contains("sFreq") && fields.size()==2)
                sFreq = fields.at(1).toDouble();

            //Read filter order
            if(line.contains("name"))
                for(int i=1; i<fields.size(); i++)
                    name.append(fields.at(i));

            //Read the filter order
            if(line.contains("order") && fields.size()==2)
                order = fields.at(1).toInt();

            //Read the filter type
            if(line.contains("type") && fields.size()==2)
                type = fields.at(1);

        } else // Read filter coefficients
            coefficientsTemp.push_back(fields.first().toDouble());
    }

    if(order != coefficientsTemp.size())
        order = coefficientsTemp.size();

    coefficients = RowVectorXd::Zero(coefficientsTemp.size());
    for(int i=0; i<coefficients.cols(); i++)
        coefficients(i) = coefficientsTemp.at(i);

    file.close();

    return true;
}


//*************************************************************************************************************

bool LoadFilter::writeFilter(const QString &path, const RowVectorXd &coefficients, const QString &type, const QString &name, const int &order, const double &sFreq)
{
    // Open file dialog
    if(!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            qDebug()<<"Error opening filter txt file for writing";
            return false;
        }

        //Write coefficients to file
        QTextStream out(&file);

        out << "#sFreq " << sFreq << "\n";
        out << "#name " << name << "\n";
        out << "#type " << type << "\n";
        out << "#order " << order << "\n";

        for(int i = 0 ; i<coefficients.cols() ;i++)
            out << coefficients(i) << "\n";

        file.close();

        return true;
    }

    qDebug()<<"Error Filter File path is empty";

    return false;
}