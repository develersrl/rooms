#!/usr/bin/env python

from origin import OriginData

class Image(OriginData):

    tag_name = 'img'

    def __init__(self, file):
        super(Image, self).__init__()
        self.file = file
