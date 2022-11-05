#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys
from PyQt5.QtWidgets import QWidget, QApplication, QSizePolicy                 ,QSlider,QHBoxLayout, QVBoxLayout
from PyQt5.QtCore import Qt, QTime, QTimer, QStringListModel, QPointF, QPoint            , pyqtSignal, QObject
from PyQt5.QtGui import QPainter, QPen, QFont, QRadialGradient, QColor, QBrush


from math import cos, sin, pi


class QNeedleIndicator(QWidget):

    def __init__(self, minVal = 0, maxVal = 100, majorTicks = 11, minorTicks = 8, gap_angle = 90):
        super().__init__()
        self.majorTicks = majorTicks
        self.minorTicks = minorTicks
        self.gap_angle   = gap_angle
        self.start_angle = 90  + self.gap_angle / 2
        self.stop_angle  = 360 - self.gap_angle
        self.rot_deg =  self.stop_angle/( self.majorTicks-1)
        self.rot_rad = ( self.rot_deg/360.0)*2*pi
        self.offset = 0
        self.wmax     = 100
        self.wmin     = 0
        self.setRange(minVal, maxVal)
        self.value   = 0
        self.step    = self.wmax / (self.majorTicks-1)
        self.labelOffset = 60
        self.digitFont = QFont("Serif", 8)
        self.labelFont = QFont("Serif", 12, QFont.DemiBold)
        self.label = "LABEL"
        self.scaleFormat = "{0:.2f}"
        self.labelFormat = "{0:.2f}"
        #self.animated = False
        #self.timer   = QTimer()
        #self.timer.setInterval(100)
        #self.timer.timeout.connect(self.animate)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.setMinimumSize(200,200)
        self.currValue = 0



    #def animate(self):    # qreal p,s,f
        #self.animFrame += 1
        ## Divide value delta on pieces. Pieces length decrease.
        #p = 10.0/(self.animFrame)
        #s = 0
        #for i in range(1,10): s += 10.0/i
        #f=p/s
        #self.currValue+=(self.valueDelta*f)
        #if self.animFrame >= 10:
            #self.timer.stop()
            #self.currValue = self.value  # lock the needle after animation on final position

        #self.update()


    def getValue(self, void):
        return self.value


    #def startAnimation(self):
        #self.animFrame = 0
        #self.valueDelta = self.value-self.currValue;   # we need to move needle by valueDelta
        #if not self.timer.isActive():        # fire timer; it will start moving the needle
            #self.timer.start()             # from currValue to value


    def resizeEvent(self, event):
        QWidget(self).resizeEvent(event)


    def paintEvent (self, event):
        self.drawBackground()
        self.drawNeedle()
        QWidget(self).paintEvent(event)


    def setMajorTicks(self, t):
        self.majorTicks = t
        self.step = (self.wmax-self.wmin)/(self.majorTicks-1)
        self.rot_deg = self.stop_angle/(self.majorTicks-1)
        self.rot_rad = (self.rot_deg/360.0)*2*pi
        self.update()


    def setMinorTicks(self, t):
        self.minorTicks = t
        self.update()


    def setDigitFont(self, f):
        self.digitFont = f
        self.update()


    def setLabelFont(self, f):
        self.labelFont = f
        self.update()


    #def setAnimated(self, anim):
        #self.animated = anim


    #def isAnimated(self):
        #return self.animated


    def setValue(self, v):
        if(v > self.wmax): v = self.wmax    # coerce
        if(v < self.wmin): v = self.wmin
        self.currValue = self.value = v
        self.update()

        #if( self.animated ):       # fire animation
            #self.value = v
            #self.startAnimation()
        #else:                # instant update




    def setLabel(self, l):
        self.label = l
        self.update()


    def setDigitFormat(self, Tformat):
        self.scaleFormat = Tformat


    def setRange(self, mi, ma):
        self.wmin  = mi
        self.wmax  = ma
        self.step = (self.wmax-self.wmin)/(self.majorTicks-1)
        self.update()


    def setMinValue(self, mi):
        self.setRange(self.mi, self.wmax)


    def setMaxValue(self, ma):
        self.setRange(self.wmin, ma)


    def setGapAngle(self, gap):
        self.gap_angle   = gap
        self.start_angle = 90 + self.gap_angle/2
        self.stop_angle  = 360 - self.gap_angle
        self.rot_deg = self.stop_angle/(self.majorTicks-1)
        self.rot_rad = (self.rot_deg/360.0)*2*pi
        self.update()


    def setLabelOffset(self, offset):
        if( self.offset < 0 ): self.offset = 0
        if( self.offset > 1 ): self.offset = 1
        #self.labelOffset = 120*offset
        self.labelOffset = 115*offset
        self.update()


    def value2angle(self, val):
        intervalLength = self.wmax-self.wmin
        val = val-self.wmin

        if( val > intervalLength ): val = intervalLength
        return self.start_angle + (val/intervalLength)*self.stop_angle


    def drawNeedle(self):
        side = min( self.width(),  self.height())
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.translate(self.width() / 2, self.height() / 2)
        painter.scale(side / 256.0, side / 256.0)
        painter.save()
        painter.rotate(self.value2angle(self.currValue))
        painter.setBrush(Qt.red)
        painter.drawConvexPolygon(QPointF(0,-2), QPointF(100, 0), QPointF(0,2), QPointF(-30,5), QPointF(-30,-5))
        painter.setBrush(Qt.black)
        painter.drawEllipse(QPoint(0,0), 5,5)
        painter.restore()



    def drawBackground(self):
        side = min( self.width(),  self.height())
        ''' Keep side size an even number by trunkating odd pixel '''
        side &= ~0x01


        gradient = QRadialGradient()

        painter  = QPainter(self)

        pen = QPen(Qt.black)
        pen.setWidth(1)

        ''' Initialize painter '''
        painter.setRenderHint(QPainter.Antialiasing)
        painter.translate(self.width() / 2, self.height() / 2)
        painter.scale(side / 256.0, side / 256.0)
        painter.setPen(pen)
        ''' Draw external circle '''
        gradient = QRadialGradient (QPointF(-128,-128), 384, QPointF(-128,-128))
        gradient.setColorAt(0, QColor(224,224,224))
        gradient.setColorAt(1, QColor(28,28,28))
        painter.setPen(pen)
        painter.setBrush(QBrush(gradient))

        painter.drawEllipse(QPoint(0,0),125,125)

        ''' Draw inner circle '''
        gradient = QRadialGradient(QPointF(128,128), 384, QPointF(128,128))
        gradient.setColorAt(0, QColor(224,224,224))
        gradient.setColorAt(1, QColor(28,28,28))
        painter.setPen(Qt.NoPen)
        painter.setBrush(QBrush(gradient))
        painter.drawEllipse(QPoint(0,0),118,118)
        ''' Draw inner shield '''
        gradient = QRadialGradient (QPointF(-128,-128), 384, QPointF(-128,-128))
        gradient.setColorAt(0, QColor(255,255,255))
        gradient.setColorAt(1, QColor(224,224,224))
        painter.setBrush(gradient)
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(QPoint(0,0), 115, 115)
        painter.setPen(pen)
        painter.setBrush(Qt.black)
        painter.drawPoint(0,0)

        line = 10;

        ''' Draw scale majorTicks using coords rotation '''
        painter.save()
        painter.setBrush(Qt.black)
        painter.rotate(self.start_angle);      #          ''' initial angle (first tick) '''
        painter.setBrush(QBrush(Qt.black))
        t_rot = self.stop_angle/(self.minorTicks*(self.majorTicks-1)+self.majorTicks-1)
        for i in range(int((self.minorTicks)*(self.majorTicks-1)+self.majorTicks)):
            if self.minorTicks:
                if (self.minorTicks+1) == 0 :
                    painter.drawLine(QPoint(105,0), QPoint(105-line, 0))
                else:
                    painter.drawLine(QPoint(105,0), QPoint(105-line/3, 0))
            else:
                painter.drawLine(QPoint(105,0), QPoint(105-line, 0))
            painter.rotate(t_rot)
        painter.restore()

        ''' Draw scale numbers using vector rotation '''
        ''' x' = xcos(a)-ysin(a)                     '''
        ''' y' = xsin(a)-ycos(a)                     '''
        painter.save()
        rotation = (self.start_angle/360)*2*pi   #       ''' Initial rotation '''
        painter.setFont(self.digitFont)
        for i in range(int(self.majorTicks)):
            point = QPointF((70*cos(rotation)), 70*sin(rotation))
            value = self.scaleFormat.format(self.wmin+i*self.step)
            size = painter.fontMetrics().size(Qt.TextSingleLine, value)
            point.setX(point.x() - size.width()/2) #  += int()
            point.setY(point.y() + size.height()/4 )
            painter.drawText(point, value)
            rotation+=self.rot_rad;

        painter.restore()

        labela = self.labelFormat.format(self.value)
        painter.setFont(self.labelFont)
        point = QPointF()
        size = painter.fontMetrics().size(Qt.TextSingleLine, labela)
        point.setX(point.x() - size.width()/2)
        point.setY(point.y() + size.height()/4 + self.labelOffset)
        painter.drawText(point, labela)

        if len(self.label) > 0: # Draw meter label
            painter.setFont(self.labelFont)
            point = QPointF()
            size = painter.fontMetrics().size(Qt.TextSingleLine, self.label)
            point.setX(point.x() - size.width()/2)
            point.setY(point.y() + size.height()/4 + self.labelOffset + 24)
            painter.drawText(point, self.label)






#class Communicate(QObject):

    #updateBW = pyqtSignal(int)




class Example(QWidget):

    def __init__(self):
        super().__init__()





        self.w1 = QNeedleIndicator(minVal = 0, maxVal = 18, majorTicks = 11, minorTicks = 8, gap_angle = 90)
        self.w1.setLabel("Voltage")
        self.w1.setValue(12)


        self.w2 = QNeedleIndicator(minVal = 0, maxVal = 10, majorTicks = 5, minorTicks = 4, gap_angle = 200)
        #self.w2.setAnimated(False)
        #self.w2.setRange(0,10)
        #self.w2.setMajorTicks(5)
        #self.w2.setMinorTicks(4)
        #self.w2.setGapAngle(200)
        self.w2.setLabelOffset(0.4)
        self.w2.setLabel("Current")
        self.w2.setDigitFont(QFont("Fixed", 10, QFont.Bold))
        self.w2.setDigitFormat("{0:.1f}")


        self.w3 = QNeedleIndicator(minVal = 0, maxVal = 150, majorTicks = 11, minorTicks = 0, gap_angle = 120)
        #self.w3.setAnimated(False)
        #self.w3.setRange(0,150)
        self.w3.setDigitFormat("{0:.0f}")
        #self.w3.setMajorTicks(11)
        #self.w3.setMinorTicks(0)
        self.w3.setGapAngle(120)
        self.w3.setLabel("Pressure")
        self.w3.setValue(60)


        self.w4 = QNeedleIndicator(minVal = -1, maxVal = 1, majorTicks = 11, minorTicks = 8, gap_angle = 120)
        #self.w4.setAnimated(False)
        #self.w4.setRange(-1, 1)
        #self.w4.setMajorTicks(11)
        #self.w4.setMinorTicks(8)
        self.w4.setLabel("Diff V")
        self.w4.setValue(0)




        hbox = QHBoxLayout()

        hbox.addWidget(self.w1)
        hbox.addWidget(self.w2)
        hbox.addWidget(self.w3)
        hbox.addWidget(self.w4)

        vbox = QVBoxLayout()
        vbox.addStretch(1)
        vbox.addLayout(hbox)
        self.setLayout(vbox)

        self.setWindowTitle('NEEDLE')
        self.show()



        #sld = QSlider(Qt.Horizontal, self)
        #sld.setFocusPolicy(Qt.NoFocus)
        #sld.setRange(0, 100)
        #sld.setValue(50)
        #sld.setGeometry(10, 10, 150, 30)

        #self.c = Communicate()
        #self.wid = QNeedleIndicator()



        #self.c.updateBW[int].connect(self.wid.setValue)

        #sld.valueChanged[int].connect(self.changeValue)




    #def changeValue(self, value):

        #self.c.updateBW.emit(value)
        #self.wid.repaint()


if __name__ == '__main__':

    app = QApplication(sys.argv)
    ex = Example()
    sys.exit(app.exec_())


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = QNeedleIndicator()
    ex.show()
    sys.exit(app.exec_())

