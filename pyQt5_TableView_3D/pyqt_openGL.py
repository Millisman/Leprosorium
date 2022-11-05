#!/usr/bin/python3
# -*- coding: utf-8 -*-

from PyQt5 import QtWidgets
from PyQt5.QtGui import QColor, QFont
from PyQt5.QtCore import Qt
from PyQt5.QtOpenGL import QGL, QGLFormat, QGLWidget
from OpenGL.GL import *
from OpenGL.GLU import *



class GLWidget(QGLWidget):
    
    def __init__(self, *args, **kwargs):
        super(GLWidget, self).__init__(*args, **kwargs)
        self.tableData = 0
        self.setAutoFillBackground(False)
        self.mouseDown = False
        self.xrot = -65
        self.yrot = 45
        self.zrot = -3.0
        self.xAxis = []
        self.yAxis = []
        self.values = {}
        self.maxCalcedValue = 10


    def mouseReleaseEvent(self, event):
        self.mouseDown = False
 
    def mousePressEvent(self, event):
        self.lastPos = event.pos()
        self.mouseDown = True

    def mouseMoveEvent(self, event):
        if not self.mouseDown: return
        self.update()
        dx = event.x() - self.lastPos.x()
        dy = event.y() - self.lastPos.y()
        if event.buttons() & Qt.LeftButton:
            self.xrot += dy
            self.yrot += dx
        elif event.buttons() & Qt.RightButton:
            self.xrot += dy
            self.zrot += dx
        self.lastPos = event.pos()

    def initializeGL(self):
        self.makeCurrent()
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glClearColor(0, 0, 0, 0) # , 0, 0, 0
        glShadeModel(GL_SMOOTH)
        glEnable (GL_LINE_SMOOTH)
        glEnable (GL_MULTISAMPLE)
        glEnable (GL_BLEND)
        glEnable (GL_POLYGON_SMOOTH)
        glHint (GL_LINE_SMOOTH_HINT,    GL_NICEST)
        glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST)
        glEnable(GL_DEPTH_TEST)
        glViewport(0, 0, self.width(), self.height() )
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(65.0, 1.0, 0.1, 4)
        glMatrixMode(GL_MODELVIEW)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
        glTranslatef(0, 0, -2)            # Move back 3 units
        glRotatef(self.xrot, 1.0, 0.0, 0.0) # Rotate it up/down
        glRotatef(self.yrot, 0.0, 0.0, 1.0) # Rotate it left/right
        # glRotatef(self.zrot, 0.0, 1.0, 0.0)
        
        glTranslatef(-0.5, -1 + (len(self.yAxis) / len(self.xAxis))*0.5, -0.5) # Center the graph in view
        # float minx = 0
        maxx = 1.0
        miny = 0.0
        maxy = 1.0
        if len(self.xAxis) < len(self.yAxis):
            maxx = maxy * (len(self.yAxis) / len(self.xAxis))
        else:
            maxy = maxx * (len(self.xAxis) / len(self.yAxis))
        
        glColor4f(1, 1, 1, 1)

        levels = 4
        font = QFont("Times", 12, QFont.Bold)
        
        for i in range(levels):
            
            glBegin(GL_LINE_STRIP)
            
            #glBegin(GL_LINES)
            
            #glVertex3f(maxx-((1)/(len(self.xAxis))),0,i/levels)
            glVertex3f(maxx, 0, i/levels)
            #glVertex3f(maxx-((1)/(len(self.xAxis))),maxy+(2)/(len(self.xAxis)),i/levels)
            glVertex3f(maxx, maxy, i/levels)
            #glVertex3f(0,maxy+(2)/(len(self.xAxis)),i/levels)
            glVertex3f(0, maxy, i/levels)
            
            glEnd()
            
            # FORMATTING
            
            #self.qglColor(Qt.white)
            txt = '{0:.2f}'.format((self.maxCalcedValue) * (i/levels))
            
            #QString::number(,'f',2)
            
            #self.qglColor(Qt.white)
            #self.renderText(-0.35, 0.4, 0.0, "Multisampling enabled")
            #self.renderText(0.15, 0.4, 0.0, "Multisampling disabled")
            
            #self.renderText(0.2, maxy + 0.2, i/levels, txt)
            
            glPushAttrib(GL_CURRENT_BIT)
            glColor4f(0.9, 0.9, 0.9, 0.9)
            self.renderText(-0.1, maxy + 0.1, i/levels, txt, font)
            glPopAttrib() # This sets the colour back to its original value
            
            


        # Longer X axis (RPM)
        for i in range(len(self.xAxis)):
            glBegin(GL_LINES)
            tmpf2=0.0
            tmpf2 = (i * maxy)/(len(self.xAxis))
            glVertex3f(maxx, tmpf2 * (maxx / 1.0), 0)
            glVertex3f(maxx, tmpf2 * (maxx / 1.0), 1)
            glEnd()
            self.renderText(0, tmpf2, -0.05, str(self.xAxis[i]))


        # Shorter Y Axis (KPA)
        for i in range(len(self.yAxis)):

            glBegin(GL_LINES)
            tmpf1 = (i * maxx)/(len(self.yAxis))
            glVertex3f(tmpf1 * (maxx / 1.0), maxy, 0)
            glVertex3f(tmpf1 * (maxx / 1.0), maxy, 1)
            glEnd()
            self.renderText(0, tmpf1, -0.05, str(self.yAxis[i]))
            


        # Middle vertical
        glBegin(GL_LINES)
        glVertex3f(maxx, maxy, 0)
        glVertex3f(maxx, maxy, 1)
        glEnd()

        # Line around the top of the graph
        glBegin(GL_LINE_STRIP)
        glVertex3f(0, maxy, 1)
        glVertex3f(maxx, maxy, 1)
        glVertex3f(maxx, 0, 1)
        glEnd()


        # Square outline around the bottom of the graph
        glBegin(GL_LINE_STRIP)
        glVertex3f(0, maxy, 0)
        glVertex3f(0, 0, 0)
        glVertex3f(maxx, 0, 0)
        glVertex3f(maxx, maxy, 0)
        glVertex3f(0, maxy, 0)
        glEnd()
        

        for x in range(len(self.xAxis)-1):
        
            for i in range(3):
                if i == 0 or i == 2:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
                    # glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
                    glBlendFunc(GL_ONE, GL_ZERO)
                    glLineWidth(1.25)
                
                elif i == 1:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
                    glBlendFunc(GL_ONE, GL_ZERO)
                    glLineWidth(1.25)
                
                glBegin(GL_QUADS)
                
                for y in range(len(self.yAxis)-1):
                    r = 0.0 ; g = 0.0 ; b = 0.0
                    val = self.values[y, x]
                    if val < self.maxCalcedValue/4:
                        g = 255*(val/(self.maxCalcedValue/4))
                        b = 255; r = 0
                    elif val < (self.maxCalcedValue/4)*2:
                        r = 0; g = 255
                        b = 255-(255*((val-((self.maxCalcedValue/4)))/(self.maxCalcedValue/4)))
                    elif val < (self.maxCalcedValue/4)*3:
                        r = 255*((val-((self.maxCalcedValue/4)*2))/(self.maxCalcedValue/4))
                        g = 255; b = 0
                    else:
                        r = 255; b = 0
                        g = 255-(255*((val-((self.maxCalcedValue/4)*3))/(self.maxCalcedValue/4)))
                    if i == 0 or i == 2:
                        r = 0; g = 0; b = 0
                    elif i == 1:
                        r = r / 255 ; g = g / 255 ; b = b / 255
                    if i==0:   m = 0.001
                    elif i==2: m = -0.001
                    else:      m = 0.0

                    # X and Y are reversed here, to allow for the graph to look the proper way.
                    y0 = (x * maxy) / (len(self.xAxis)-1)
                    x0 = y / (len(self.yAxis)-1)
                    z0 = self.values[y, x]  / self.maxCalcedValue
                    glColor4f(r, g, b, 1)
                    glVertex3f(x0, maxy-y0, z0 + m)

                    y1 =          (x * maxy) / (len(self.xAxis)-1)
                    x1 =               (y+1) / (len(self.yAxis)-1)
                    z1 = self.values[y+1, x] / self.maxCalcedValue
                    glColor4f(r, g, b, 1)
                    glVertex3f(x1, (maxy-y1), z1 + m)

                    y2 =      ((x+1) * maxy) / (len(self.xAxis)-1)
                    x2 =               (y+1) / (len(self.yAxis)-1)
                    z2 = self.values[y+1, x+1] / self.maxCalcedValue
                    glColor4f(r, g, b, 1)
                    glVertex3f(x2, (maxy-y2), z2 + m)

                    y3 =      ((x+1) * maxy) / (len(self.xAxis)-1)
                    x3 =                   y / (len(self.yAxis)-1)
                    z3 = self.values[y, x+1] / self.maxCalcedValue
                    glColor4f(r, g, b, 1)
                    glVertex3f(x3, (maxy-y3), z3 + m)

                glEnd()



    def resizeGL(self, width, height):
        glViewport(0, 0, width, height)
        glMatrixMode(GL_MODELVIEW)




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

if __name__ == '__main__':
    app = QtWidgets.QApplication([])
    
    
    Columns = [16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1] # 16 column
    Rows    = [600,720,840,990,1170,1380,1650,1950,2310,2730,3210,3840,4530,5370,6360,7500] # 16 row
    MMM = [ZZZ[i:i + 16] for i in range(0, len(ZZZ), 16)] # 16 * 16
    
    
    w = GLWidget()
    
    w.xAxis = Rows 
    w.yAxis = Columns   
    
    for column in range(len(Columns)):
        for row in range(len(Rows)):
            w.values[row, column] = MMM[row][column]/2
            
    w.maxCalcedValue = max(ZZZ)/2
    
    w.show()
    
    app.exec_()




''''
    VERTEX_SHADER = """
        #extension GL_ARB_explicit_attrib_location : enable
        attribute vec4 in_position;
        attribute vec2 in_tex_coord;
        varying vec2 vs_tex_coord;

        void main(void){
            gl_Position = in_position;
            vs_tex_coord = in_tex_coord;
        }"""

    FRAGMENT_SHADER = """
        uniform sampler2D tex;
        varying vec2 vs_tex_coord;

        void main(void){
            gl_FragColor = texture2D(tex, vs_tex_coord)
        }"""

    vbov = vbo.VBO(np.array([[-1.0, -1.0, 0.0, 1.0, 0.0, 0.0],
                             [1.0, -1.0, 0.0, 1.0, 1.0, 0.0],
                             [1.0, 1.0, 0.0, 1.0, 1.0, 1.0],
                             [-1.0, 1.0, 0.0, 1.0, 0.0, 1.0]], 'f'))

'''
