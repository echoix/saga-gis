#! /usr/bin/env python

#################################################################################
# MIT License

# Copyright (c) 2023 Olaf Conrad

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#################################################################################

#_________________________________________
##########################################

__bibtex__ = r"""@Article{gmd-8-1991-2015,
AUTHOR  = {Conrad, O. and Bechtel, B. and Bock, M. and Dietrich, H. and Fischer, E. and Gerlitz, L. and Wehberg, J. and Wichmann, V. and B\"ohner, J.},
TITLE   = {System for Automated Geoscientific Analyses (SAGA) v. 2.1.4},
JOURNAL = {Geoscientific Model Development},
VOLUME  = {8},
YEAR    = {2015},
NUMBER  = {7},
PAGES   = {1991--2007},
URL     = {https://gmd.copernicus.org/articles/8/1991/2015/},
DOI     = {10.5194/gmd-8-1991-2015}
}"""


#_________________________________________
##########################################

import os

SAGA_Path = None
if os.name == 'nt': # Windows
	SAGA_Path = os.getenv('SAGA_PATH')
	if not SAGA_Path:
		SAGA_Path = os.path.split(os.path.dirname(__file__))[0]

	if 'add_dll_directory' in dir(os):
		os.add_dll_directory(SAGA_Path)
	else:
		os.environ['PATH'] = SAGA_Path + ';' + os.environ['PATH']


#_________________________________________
##########################################

AutoLoadTools = True

from PySAGA import saga_api

saga_api.SG_Initialize_Environment(AutoLoadTools, True, SAGA_Path)


#_________________________________________
##########################################

def Version():
	import sys
	saga_api.SG_UI_Msg_Add(saga_api.CSG_String('_______'), True)
	saga_api.SG_UI_Msg_Add(saga_api.CSG_String('Python-' + sys.version), True)
	saga_api.SG_UI_Msg_Add(saga_api.CSG_String('SAGA-{:s} (loaded {:d} libraries, {:d} tools)'.format(saga_api.SAGA_VERSION,
		saga_api.SG_Get_Tool_Library_Manager().Get_Count(),
		saga_api.SG_Get_Tool_Library_Manager().Get_Tool_Count()
	), True)
	saga_api.SG_UI_Msg_Add(saga_api.CSG_String('_______'), True)

	return True


#_________________________________________
##########################################
