#!/usr/bin/env python3.5
#Zcode PyQt
#not comprited yet
"""-----------------------"""
""" GUI program for QST-  """
""" Use: Python3, PyQt5   """
"""-----------------------"""

""" import some Widgets """
import os
import sys
from PyQt5.QtWidgets import (QWidget, QCheckBox, QComboBox, QLabel, QApplication, QPushButton, QGridLayout, QMessageBox, QLineEdit, QDialog)
from PyQt5.QtCore import Qt
from datetime import datetime
import subprocess
from subprocess import Popen

""" make main window (button, checkbox, combobox, layout) """
class ButtonBoxWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI_set()

        def initUI_set(self):

            """ QComboBox is a widget that allows a user to choose from a list of options. """

            """ make detector combobox """
#---Wave_dump configuration file
        global combo_card
        combo_card = QComboBox(self)
        combo_card.addItem("V1730B_Borad1")
        combo_card.addItem("V1730B_Board2")
#---Difine run mode
        global combo_mode
        combo_mode = QComboBox(self)
        combo_mode.addItem("Test Run")
        combo_mode.addItem("Calibration Run")
        combo_mode.addItem("Physics Run")
        combo_mode.currentIndexChanged.connect(self.ChangeMode)
#------Edite ard-----"""
        """ QLabel is a widget that  """
        mode = QLabel("Run mode")
        #beam = QLabel("Beam")
        #RI = QLabel("RI")
#---Comment 
        global comment
        comment = QLineEdit(self)
        comment_label = QLabel("Rnu cmment :")
#---Botton difintion
        global run
        run = QPushButton("Run", self)
        run.setStyleSheet("background-color: rgb(134,218,255)")
        run.setEnabled(False)
        run.clicked.connect(self.RunbuttonClicked)
        global quit_button
        quit_button = QPushButton("Quit", self)
        quit_button.setEnabled(False)
        quit_button.clicked.connect(QApplication.instance().quit)
#---function whti pottan----"""
        global run_enabled
        run_enabled = QCheckBox("Ready?", self)
        run_enabled.stateChanged.connect(self.ChangeState_Ready)
        global quit_enabled
        quit_enabled = QCheckBox("Quit",self)
        quit_enabled.stateChanged.connect(self.ChangeState_Quit)

#--- Layout-----
        global layout
        layout = QGridLayout()
        layout.addWidget(mode, 1, 4)
        layout.addWidget(combo_mode, 1, 5)
        layout.addWidget(combo_card, 2, 5)
        layout.addWidget(comment_label, 9, 0)
        layout.addWidget(comment, 10, 0, 2, 7)
        layout.addWidget(edit_enabled, 12, 5)
        layout.addWidget(run_enabled, 12, 6)
        layout.addWidget(quit_enabled, 12, 0)
        layout.addWidget(edit, 13, 5)
        layout.addWidget(run, 13, 6)
        layout.addWidget(quit_button, 13, 0)
        self.setLayout(layout)
        self.setGeometry(0, 650, 700, 400)
        self.show()
#--- Check box difinition
        def ChangeState_Ready(self, state):
            if state == Qt.Checked:
                run.setEnabled(True)
            else:
                run.setEnabled(False)

        def ChangeState_Edit(self, state):
            if state == Qt.Checked:
                edit.setEnabled(True)
            else:
                edit.setEnabled(False)

        def ChangeState_Quit(self, state):
            if state == Qt.Checked:
                quit_button.setEnabled(True)
            else:
                quit_button.setEnabled(False)

#---Edite WaveDump configlation file
        def EditbuttonClicked(self):
            edit.setEnabled(False)
            card_index = combo_card.currentIndex()
            if card_index == 0:
                subprocess.call(["vi", "/home/quser/QST_EXP/daq/KPSI_V1730_DAQ/wave_source_code/Conf/WaveDumpConfig1.txt"])
            else:
                subprocess.call(["vi", "/home/quser/QST_EXP/daq/KPSI_V1730_DAQ/wave_source_code/Conf/WaveDumpConfig2.txt"])
            edit.setEnabled(True)
        def StopbuttonClicked(self):
            edit_enabled.setChecked(False)
###----Execute run controler -------
            comment_info = comment.txt();
            mode_info = str(combo_card.currentIndex());
            information = [mode.info,comment.txt]
            subprocess.call(["sh", "daq_control.sh", *information])
            subwindow = InformationWindow()
            subwindow.show()
            #cmd = "sh /home/assy/Work/E525/daq/daq_control.sh"
            #Popen( cmd .strip().split(" "))
            #run.setEnabled(True)
            run_enabled.setChecked(False)

        def closeEvent(self, event):
            reply = QMessageBox.question(self, "Message", "Are you sure to quit?", QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
            if reply == QMessageBox.Yes:
                event.accept()
            else:
                event.ignore()

class InformationWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI_info()

        def initUI_info(self, parent=None):
            self.w = QDialog(parent)
            label_info = QLabel("Run information")
            information = QLabel("hogehoge")
            start_time_label = QLabel("Acquisition start")
            get_events = QPushButton("Current events", self)
            get_events.clicked.connect(self.GetCurrentEvents)
            lines = QLabel()
            plot = QPushButton("Plot", self)
            acquisition_start = QPushButton("Acquisition start", self)
            quit = QPushButton("Quit", self)

            layout = QGridLayout()
            layout.addWidget(label_info, 0, 0)
            layout.addWidget(information, 1, 0)
            layout.addWidget(start_time_label, 2, 0)
            layout.addWidget(get_events, 3, 0)
            layout.addWidget(lines, 3, 1)
            layout.addWidget(plot, 4, 0)
            layout.addWidget(acquisition_start, 5, 0)
            layout.addWidget(quit, 6, 0)
            self.w.setLayout(layout)
            self.setGeometry(200, 200, 300, 400)

        def show(self):
            self.w.exec_()


        def GetCurrentEvents(self):
            lines = subprocess.call(["wc", "-l", "wave0.txt"])
            self.SendEvents(lines)

        def SendEvents(self, lines):
            self.lines = lines

def main():
    app = QApplication(sys.argv)
    #os.chdir("/home/quser/QST_EXP/daq/KPSI_DAQ/wave_source_code/")
    btn = ButtonBoxWidget()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
