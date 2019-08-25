#npr.py
import imageIO as io
#import a2
import numpy as np
import scipy as sp
from scipy import signal
from scipy import ndimage
import random as rnd
import nprHelper as helper
import math

def brush(out, y, x, color, texture):
    '''out: the image to draw to. y,x: where to draw in out. color: the color of the stroke. texture: the texture of the stroke.'''

def singleScalePaint(im, out, importance, texture, size=10, N=1000, noise=0.3):
    '''Paints with all brushed at the same scale using importance sampling.'''

def painterly(im, texture, N=10000, size=50, noise=0.3):
    '''First paints at a coarse scale using all 1's for importance sampling, then paints again at size/4 scale using the sharpness map for importance sampling.'''

def computeAngles(im):
    '''Return an image that holds the angle of the smallest eigenvector of the structure tensor at each pixel. If you have a 3 channel image as input, just set all three channels to be the same value theta.'''

def singleScaleOrientedPaint(im, out, thetas, importance, texture, size, N, noise, nAngles=36):
    '''same as single scale paint but now the brush strokes will be oriented according to the angles in thetas.'''

def orientedPaint(im, texture, N=7000, size=50, noise=0.3):
    '''same as painterly but computes and uses the local orientation information to orient strokes.'''
