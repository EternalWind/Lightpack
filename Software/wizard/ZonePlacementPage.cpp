/*
 * ZoneConfiguration.cpp
 *
 *  Created on: 10/25/2013
 *     Project: Prismatik
 *
 *  Copyright (c) 2013 Tim
 *
 *  Lightpack is an open-source, USB content-driving ambient lighting
 *  hardware.
 *
 *  Prismatik is a free, open-source software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Prismatik and Lightpack files is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ZonePlacementPage.hpp"
#include "ui_ZonePlacementPage.h"
#include "QDesktopWidget"
#include "AbstractLedDevice.hpp"
#include "Settings.hpp"
#include "AndromedaDistributor.hpp"
#include "GrabAreaWidget.hpp"
#include "LedDeviceLightpack.hpp"


ZonePlacementPage::ZonePlacementPage(SettingsScope::Settings *settings, QWidget *parent) :
    QWizardPage(parent),
    SettingsAwareTrait(settings),
    _ui(new Ui::ZonePlacementPage)
{
    _ui->setupUi(this);
}

ZonePlacementPage::~ZonePlacementPage()
{
    delete _ui;
}

void ZonePlacementPage::initializePage()
{
    _screenId = field("screenId").toInt();
    _ledDevice = new LedDeviceLightpack(wizard());
    _ledDevice->open();
    _ledDevice->setSmoothSlowdown(70);
    /*
     *TODO: create ledDevice
     */
//    size_t ledsCount = _ledDevice->maxLedsCount();
//    for(size_t i = 0; i < ledsCount; i++) {

//    }
}

bool ZonePlacementPage::validatePage()
{
    for(size_t i = 0; i < _zones.size(); i++) {
        delete _zones[i];
    }
    delete _ledDevice;
    return true;
}

void ZonePlacementPage::on_pbAndromeda_clicked()
{
    AndromedaDistributor *andromeda = new AndromedaDistributor(_screenId, true, 10);

    for(size_t i = 0; i < 10; i++) {
        ScreenArea *sf = andromeda->next();
        qDebug() << sf->hScanStart() << sf->vScanStart();
        GrabAreaWidget *zone = new GrabAreaWidget(i);

        QRect s = QApplication::desktop()->screenGeometry(_screenId);
        QRect r(s.left() + sf->hScanStart() * (float)s.width(),
                s.top()  + sf->vScanStart() * (float)s.height(),
                (sf->hScanEnd() - sf->hScanStart()) * (float)s.width(),
                (sf->vScanEnd() - sf->vScanStart()) * (float)s.height());
        zone->move(r.topLeft());
        zone->resize(r.size());
        connect(zone, SIGNAL(resizeOrMoveStarted(int)), this, SLOT(turnLightOn(int)));
        connect(zone, SIGNAL(resizeOrMoveCompleted(int)), this, SLOT(turnLightsOff(int)));
        zone->show();
        _zones.append(zone);

        delete sf;
    }
    delete andromeda;

}

void ZonePlacementPage::turnLightOn(int id)
{
    QList<QRgb> lights;
    for(size_t i=0; i < _ledDevice->maxLedsCount(); i++)
    {
        if (i == id)
            lights.append(qRgb(255,255,255));
        else
            lights.append(0);
    }
    _ledDevice->setColors(lights);
}

void ZonePlacementPage::turnLightsOff(int id)
{
    Q_UNUSED(id)
    QList<QRgb> lights;
    for(size_t i=0; i < _ledDevice->maxLedsCount(); i++)
    {
        lights.append(0);
    }
    _ledDevice->setColors(lights);
}