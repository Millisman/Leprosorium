#!/usr/bin/python3
# -*- coding: utf-8 -*-


from PyQt5.QtWidgets import QWidget, QApplication
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QPainter, QColor


# QPen QFont QRadialGradient QBrush QTime QTimer QPoint QObject
# , QSizePolicy,QSlider,QHBoxLayout, QVBoxLayout



class TableViewNew3D(QWidget):

    itemChangeRequest = pyqtSignal(object)
    currentSelectionChanged = pyqtSignal(object)
    keyPressed = pyqtSignal(int)


    def __init__(self, Rows, Columns, maxValue):

        super().__init__()
        self.setFocusPolicy(Qt.ClickFocus)
        self.Rows = Rows
        self.Columns = Columns
        self.maxValue = maxValue  # Число красного цвета, 0 - синий
        self.TracingRow    = 0.0 # Координаты точки трасера
        self.TracingColumn = 0.0
        self.TracingEnabled  = False # трасер включен?
        self.itH  = 30
        self.itW  = 100
        self.inEdit = False  # Клетка редактируется
        self.editText = ''   # Текст редактируемой клетки
        self.values = {} #  self.values[row, column] = 0.0
        self.CurCell_col = 0 # Координаты курсора
        self.CurCell_row = 0

        for column in range(len(self.Columns)): # column
            for row in range(len(self.Rows)):
                self.values[row, column] = 0.0 # MMM[row][column]/2
        #self.maxValue = max(ZZZ)/2



    def setTracingEnabled(self, Tracing):
        self.TracingEnabled = bool(Tracing)
        self.update()

    def setTracingValue(self, Row, Column):
        self.TracingRow = Row
        self.TracingColumn = Column
        if self.TracingEnabled is True:
            self.update()


    def resizeEvent(self, evt):
        self.itW = int(self.width()  / (len(self.Columns) + 1))
        self.itH = int(self.height() / (len(self.Rows) + 1))
        self.update()


    def drawCell(self, painter, cellx, celly, Val, highlight):
        oldpen = painter.pen()
        pen = oldpen #QPen(oldpen)

        if highlight is True:
            pen.setColor(QColor.fromRgb(0,0,255))
            pen.setWidth(2)
            painter.setPen(pen)
            painter.drawRect( cellx*self.itW+2, celly*self.itH+2, self.itW-2, self.itH-2 )
        else:
            pen.setColor(QColor.fromRgb(0,0,0))
            painter.setPen(pen)
            painter.drawRect( cellx*self.itW+1, celly*self.itH+1, self.itW-1, self.itH-1 )

        pen.setColor(QColor.fromRgb(0,0,0))


        #if type(val) == str:
        try:
            val = float(Val)
        except:
            val = 0.0
            pass


        if highlight is True or cellx == 0 or celly == len(self.Columns):
            # На данный момент отключить окраску для оси
            bgcolor = QColor.fromRgb(255, 255, 255)
            painter.fillRect( self.itW*cellx+4, self.itH*celly+4, self.itW-5, self.itH-5, bgcolor )
            if highlight is True:
                txt = '{0:.2f}'.format(val)
            else:
                txt = '{0:.0f}'.format(val)
        else:
            txt = '{0:.2f}'.format(val)

            if cellx == 0:
                vx4 = max(self.Columns)/4.0
            elif celly == len(self.Rows):
                vx4 = max(self.Rows)/4.0
            else:
                vx4 = self.maxValue/4.0

            if vx4 == 0:
                bgcolor = QColor.fromRgb(255, 255, 255)
                painter.fillRect( self.itW*cellx+2, self.itH*celly+2, self.itW-2, self.itH-2, bgcolor)
            elif val < vx4:
                bgcolor = QColor.fromRgb(0, 255 * (val/vx4), 255)
                painter.fillRect( self.itW*cellx+2, self.itH*celly+2, self.itW-2, self.itH-2, bgcolor)
            elif val < vx4*2:
                bgcolor = QColor.fromRgb(0, 255, 255 - (255 * ((val-vx4)/vx4)) )
                painter.fillRect( self.itW*cellx+2, self.itH*celly+2, self.itW-2, self.itH-2, bgcolor)
            elif val < vx4*3:
                bgcolor = QColor.fromRgb( (255*((val-(vx4*2))/vx4)), 255, 0)
                painter.fillRect( self.itW*cellx+2, self.itH*celly+2, self.itW-2, self.itH-2, bgcolor)
            else:
                bgcolor = QColor.fromRgb(255, 255 - ( 255 * ((val-((vx4)*3))/(vx4))), 0)
                painter.fillRect((self.itW*cellx)+2,(self.itH*celly)+2,self.itW-2,self.itH-2,bgcolor)

        painter.setPen(pen)

        width = painter.fontMetrics().width(txt)
        painter.drawText( cellx*self.itW + self.itW/2.0 - width/2.0,
            celly*self.itH + self.itH/2.0 - 2 + painter.fontMetrics().height()/2.0, txt)
        painter.setPen(oldpen)


    #def paintEvent(self, ev):
        #if self._img is None:
            #return
        #p = QtGui.QPainter(self)
        #p.drawImage(self.rect(), self._img, QtCore.QRect(0, 0, self._img.width(), self._img.height()))
        #p.end()
    #def paintEvent (QPaintEvent: evt)

    def paintEvent(self, ev):
        painter = QPainter(self)
        painter.setPen(QColor.fromRgb(0,0,0))
        painter.drawRect( 0, 0, self.width()-1, self.height()-1 )
        drawTraceY = 0.0
        drawTraceX = 0.0
        FoundY = False
        FoundX = False

        # Рисуем вертикальную левую шкалу, проходим по значениям заголовка столбцов
        for colum_i in range(len(self.Columns)):
            if (self.CurCell_col == -1) and (self.CurCell_row == colum_i):
                if self.inEdit: self.drawCell( painter, 0, colum_i, self.editText, True)
                else:           self.drawCell( painter, 0, colum_i, self.Columns[colum_i], True)
            else:               self.drawCell( painter, 0, colum_i, self.Columns[colum_i], False)

            painter.setPen(QColor.fromRgb(0,0,0))


            #if row_i == len(self.Rows) - 1:
                #prev = self.Rows[row_i-1]
                #curr = self.Rows[row_i]
                #nnext = curr + ((curr - prev) / 2.0)


            if self.TracingEnabled is True:

                if self.TracingColumn < self.Columns[colum_i]:
                    if colum_i == len(self.Columns) - 1:
                        curr = self.Columns[colum_i]
                        nnext = 0
                        FoundY = True
                        if self.TracingColumn < nnext:
                            drawTraceY = colum_i*self.itH+self.itH
                        else:
                            percent = ( nnext-self.TracingColumn ) / ( nnext-curr )
                            drawTraceY = ((colum_i * self.itH)+self.itH) - (percent * (self.itH/2.0))

                elif not FoundY:

                    if colum_i == 0:
                        # Значение находится между верхним и нулевым значениями
                        prev = self.Columns[colum_i] - ((self.Columns[colum_i+1]-self.Columns[colum_i])/2.0)
                        lastY = 0
                    else:
                        prev = self.Columns[colum_i-1]
                        lastY = (colum_i-1)*self.itH + (self.itH/2.0)-2

                    diff = prev - self.Columns[colum_i]
                    percent =( prev - self.TracingColumn ) / diff
                    currentY = colum_i*self.itH + (self.itH/2.0)-2
                    drawTraceY = (lastY + (percent * (currentY - lastY)))
                    if (drawTraceY < 0):
                        # Cap it at 0
                        drawTraceY = 0
                    FoundY = True




        # Рисуем горизонтальную шкалу внизу, проходим по значениям заголовка строки
        for row_i in range(len(self.Rows)):
            if (self.CurCell_row == -1) and (self.CurCell_col == row_i):
                if self.inEdit: self.drawCell( painter, row_i+1, len(self.Rows), self.editText, True)
                else:           self.drawCell( painter, row_i+1, len(self.Rows), self.Rows[row_i], True)
            else:               self.drawCell( painter, row_i+1, len(self.Rows), self.Rows[row_i], False)

            painter.setPen(QColor.fromRgb(0,0,0))

            if self.TracingEnabled:

                if self.TracingRow > self.Rows[row_i]:
                    if row_i == len(self.Rows) - 1:
                        prev = self.Rows[row_i-1]
                        curr = self.Rows[row_i]
                        nnext = curr + ((curr - prev) / 2.0)
                        FoundX = True
                        if (self.TracingRow > nnext):
                            drawTraceX = self.itW + (row_i * self.itW) + (self.itW)

                        else:
                            percent = (self.TracingRow - curr) / (nnext - curr)
                            drawTraceX = (((row_i+1) * self.itW) + (self.itW / 2.0)) + (percent * (self.itW/2.0))


                elif not FoundX:
                    if row_i == 0:
                        prev = self.Rows[row_i] - ((self.Rows[row_i+1] - self.Rows[row_i]))
                        lastx = self.itW
                    else:
                        prev = self.Rows[row_i-1]
                        lastx = self.itW + ((row_i-1) * self.itW) + (self.itW / 2.0)


                    diff = self.Rows[row_i] - prev
                    #if diff == 0: diff = 0.1
                    percent = (self.TracingRow - prev) / diff


                    currentX = self.itW + ((row_i)*self.itW) + ((self.itW/2.0))
                    drawTraceX = (lastx + (percent * (currentX - lastx)))
                    if drawTraceX < self.itW:
                        drawTraceX = self.itW
                    FoundX = True


        for row_i in range(len(self.Rows)):
            for colum_i in range(len(self.Columns)):
                if (self.CurCell_row == row_i ) and (self.CurCell_col == colum_i ):
                    if self.inEdit: self.drawCell( painter, colum_i+1, row_i, self.editText, True)
                    else:           self.drawCell( painter, colum_i+1, row_i, self.values[row_i, colum_i], True)
                else:               self.drawCell( painter, colum_i+1, row_i, self.values[row_i, colum_i], False)


        if FoundY and FoundX and self.TracingEnabled:
            pen = painter.pen()
            pen.setWidth(5)
            pen.setColor(QColor.fromRgb(255,255,0))
            painter.setPen(pen)
            painter.drawLine(0,drawTraceY,4,drawTraceY)
            painter.drawEllipse(drawTraceX-2, drawTraceY-1, 4, 2)
            pen.setColor(QColor.fromRgb(0,0,0))
            pen.setWidth(2)
            painter.setPen(pen)
            painter.drawEllipse(drawTraceX-6, drawTraceY-4, 12, 8)


    def mousePressEvent(self, evt):
        x = int(evt.x() / self.itW)
        y = int(evt.y() / self.itH)
        if y == len(self.Columns): y = -1
        self.CurCell_col = x-1
        self.CurCell_row = y
        self.inEdit = False
        self.editText = ""
        self.update()


    def keyPressEvent(self, evt):
        key = evt.key()

        if key in range(Qt.Key_0, Qt.Key_9+1):
            self.inEdit = True
            self.editText += str(key - 0x30)

            self.update()

        elif key == Qt.Key_Plus:

            if not self.inEdit:
                self.values[self.CurCell_row, self.CurCell_col] += 0.1
                if self.values[self.CurCell_row, self.CurCell_col] > self.maxValue:
                    self.values[self.CurCell_row, self.CurCell_col] = self.maxValue
            self.update()

        elif key == Qt.Key_Minus:

            if not self.inEdit:
                self.values[self.CurCell_row, self.CurCell_col] -= 0.1
                if self.values[self.CurCell_row, self.CurCell_col] < 0:
                    self.values[self.CurCell_row, self.CurCell_col] = 0
            self.update()


        elif key == Qt.Key_Period:

            if not self.inEdit:
                self.inEdit = True
                self.editText = "0"
            if len(self.editText) > 0:
                if self.editText[-1] != ".": self.editText += "."
            self.update()

        elif key == Qt.Key_Backspace:

            if self.inEdit:
                if len(self.editText) > 0: self.editText = self.editText[0:len(self.editText)-1] #  = self.editText.mid(0, self.editText.length() - 1)
                self.update()

        elif key == Qt.Key_Enter or key == Qt.Key_Return:

            if self.inEdit:
                try:
                    nv = float(self.editText)
                    if nv <= self.maxValue:
                        self.itemChangeRequest.emit((self.CurCell_row, self.CurCell_col, nv))
                        self.values[self.CurCell_row, self.CurCell_col] = nv
                        self.inEdit = False
                        self.editText = ""
                except:
                    pass
            self.update()

        elif key == Qt.Key_Escape:
            if self.inEdit:
                self.inEdit = False
                self.editText = ""
                self.update()

        elif key == Qt.Key_Up:
            if self.CurCell_row > 0 or self.CurCell_row == -1: # BUG??
                if self.inEdit:
                    self.inEdit = False
                    self.itemChangeRequest.emit((self.CurCell_row, self.CurCell_col, self.editText))
                    self.editText = ""
                if self.CurCell_row == -1:
                    self.CurCell_row = len(self.Rows) - 1
                else:
                    self.CurCell_row = self.CurCell_row - 1
                self.update()

        elif key == Qt.Key_Down:
            if self.CurCell_row < len(self.Rows) and self.CurCell_row != -1:
                if self.inEdit:
                    self.inEdit = False
                    self.itemChangeRequest.emit(( self.CurCell_row, self.CurCell_col, self.editText))
                    self.editText = ""
                if self.CurCell_row == len(self.Rows) - 1:
                    self.CurCell_row = -1
                    if self.CurCell_col == -1: # Переход на другую ось
                        self.CurCell_col = 0
                else:
                    self.CurCell_row += 1
                self.update()

        elif key == Qt.Key_Left:
            if self.CurCell_col >= 0:
                if self.inEdit:
                    self.inEdit = False
                    self.itemChangeRequest.emit(( self.CurCell_row, self.CurCell_col, self.editText ))
                    self.editText = ""
                if self.CurCell_col == 0 and self.CurCell_row == -1: # Переход на другую ось
                    self.CurCell_row = len(self.Rows) - 1
                self.CurCell_col -=  1
                self.update()

        elif key == Qt.Key_Right:
            if self.CurCell_col < len(self.Columns)-1:
                if self.inEdit:
                    self.inEdit = False
                    self.itemChangeRequest.emit(( self.CurCell_row, self.CurCell_col, self.editText ))
                    self.editText = ""
                self.CurCell_col += 1
                self.update()





ZZZ = [ 0x09,0x09,0x09,0x09,0x11,0x14,0x1A,0x20,0x26,0x24,0x30,0x33,0x3B,0x43,0x45,0x45,
        0x09,0x09,0x09,0x09,0x11,0x14,0x1A,0x20,0x26,0x24,0x30,0x33,0x3B,0x43,0x45,0x45,
        0x09,0x09,0x09,0x09,0x11,0x14,0x1A,0x20,0x26,0x24,0x30,0x33,0x3B,0x43,0x45,0x45,
        0x09,0x09,0x09,0x09,0x13,0x14,0x1A,0x20,0x26,0x26,0x32,0x33,0x3F,0x43,0x45,0x45,
        0x09,0x09,0x09,0x0A,0x13,0x16,0x1C,0x22,0x28,0x2F,0x39,0x3B,0x3F,0x47,0x49,0x49,
        0x09,0x09,0x09,0x0B,0x15,0x28,0x2C,0x35,0x3A,0x41,0x42,0x43,0x45,0x45,0x4B,0x4B,
        0x0B,0x0D,0x17,0x1D,0x25,0x2D,0x33,0x38,0x3F,0x45,0x4A,0x48,0x48,0x48,0x4A,0x4C,
        0x16,0x1A,0x22,0x26,0x2D,0x33,0x39,0x3D,0x46,0x48,0x4D,0x4A,0x4A,0x4A,0x4A,0x50,
        0x21,0x27,0x2F,0x35,0x33,0x36,0x3D,0x41,0x49,0x4B,0x4F,0x4C,0x4C,0x4C,0x4A,0x52,
        0x28,0x2E,0x3A,0x3A,0x37,0x37,0x3D,0x45,0x4C,0x4D,0x4F,0x4F,0x4F,0x52,0x52,0x56,
        0x2E,0x38,0x3E,0x40,0x38,0x37,0x45,0x49,0x4E,0x4F,0x51,0x50,0x50,0x54,0x58,0x58,
        0x30,0x3E,0x42,0x40,0x38,0x3D,0x47,0x4F,0x52,0x50,0x4F,0x4E,0x4E,0x54,0x54,0x5A,
        0x32,0x40,0x46,0x48,0x48,0x49,0x4B,0x4E,0x51,0x52,0x4E,0x4D,0x4B,0x54,0x58,0x58,
        0x2E,0x3C,0x40,0x42,0x46,0x41,0x47,0x4B,0x4E,0x4E,0x4E,0x4D,0x45,0x54,0x54,0x56,
        0x28,0x32,0x36,0x38,0x36,0x39,0x43,0x49,0x4B,0x4E,0x48,0x48,0x49,0x50,0x54,0x54,
        0x24,0x28,0x28,0x28,0x30,0x35,0x3F,0x47,0x4B,0x4E,0x47,0x46,0x48,0x4C,0x50,0x50]



#MMM = [ [0x09,0x09,0x09,0x09,0x11,0x14,0x1A,0x20,0x26,0x24,0x30,0x33,0x3B,0x43,0x45,0x45],
    #[0x09,0x09,0x09,0x09,0x11,0x14,0x1A,0x20,0x26,0x24,0x30,0x33,0x3B,0x43,0x45,0x45],
    #[0x09,0x09,0x09,0x09,0x11,0x14,0x1A,0x20,0x26,0x24,0x30,0x33,0x3B,0x43,0x45,0x45],
    #[0x09,0x09,0x09,0x09,0x13,0x14,0x1A,0x20,0x26,0x26,0x32,0x33,0x3F,0x43,0x45,0x45],
    #[0x09,0x09,0x09,0x0A,0x13,0x16,0x1C,0x22,0x28,0x2F,0x39,0x3B,0x3F,0x47,0x49,0x49],
    #[0x09,0x09,0x09,0x0B,0x15,0x28,0x2C,0x35,0x3A,0x41,0x42,0x43,0x45,0x45,0x4B,0x4B],
    #[0x0B,0x0D,0x17,0x1D,0x25,0x2D,0x33,0x38,0x3F,0x45,0x4A,0x48,0x48,0x48,0x4A,0x4C],
    #[0x16,0x1A,0x22,0x26,0x2D,0x33,0x39,0x3D,0x46,0x48,0x4D,0x4A,0x4A,0x4A,0x4A,0x50],
    #[0x21,0x27,0x2F,0x35,0x33,0x36,0x3D,0x41,0x49,0x4B,0x4F,0x4C,0x4C,0x4C,0x4A,0x52],
    #[0x28,0x2E,0x3A,0x3A,0x37,0x37,0x3D,0x45,0x4C,0x4D,0x4F,0x4F,0x4F,0x52,0x52,0x56],
    #[0x2E,0x38,0x3E,0x40,0x38,0x37,0x45,0x49,0x4E,0x4F,0x51,0x50,0x50,0x54,0x58,0x58],
    #[0x30,0x3E,0x42,0x40,0x38,0x3D,0x47,0x4F,0x52,0x50,0x4F,0x4E,0x4E,0x54,0x54,0x5A],
    #[0x32,0x40,0x46,0x48,0x48,0x49,0x4B,0x4E,0x51,0x52,0x4E,0x4D,0x4B,0x54,0x58,0x58],
    #[0x2E,0x3C,0x40,0x42,0x46,0x41,0x47,0x4B,0x4E,0x4E,0x4E,0x4D,0x45,0x54,0x54,0x56],
    #[0x28,0x32,0x36,0x38,0x36,0x39,0x43,0x49,0x4B,0x4E,0x48,0x48,0x49,0x50,0x54,0x54],
    #[0x24,0x28,0x28,0x28,0x30,0x35,0x3F,0x47,0x4B,0x4E,0x47,0x46,0x48,0x4C,0x50,0x50] ]






if __name__ == '__main__':




    import sys
    app = QApplication(sys.argv)

    Columns = [16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1] # 16 column
    Rows    = [600,720,840,990,1170,1380,1650,1950,2310,2730,3210,3840,4530,5370,6360,7500] # 16 row
    MMM = [ZZZ[i:i + 16] for i in range(0, len(ZZZ), 16)] # 16 * 16

    w = TableViewNew3D(Rows, Columns, max(ZZZ)/2)
    w.values.clear()

    for column in range(len(Columns)):
        for row in range(len(Rows)):
            w.values[row, column] = MMM[row][column]/2

    w.setTracingValue(2000, 10)
    w.setTracingEnabled(True)
    w.resize(740, 360)
    w.show()




    sys.exit(app.exec_())
