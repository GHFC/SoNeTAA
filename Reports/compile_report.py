#!/home/ramon/anaconda2/envs/mne_dev/bin/python
"""Compiling report file for the HDC evaluation test."""

import pandas as pd
import numpy as np
import os.path as op
import os
import re


def load_main():
    main_tex = op.join(op.dirname(op.abspath(__file__)),
                       'report_document/main.tex')
    with open(main_tex, 'r') as fid:
        content = fid.read()
    return content, main_tex


def change_content_main(content, subid):
    pattern = r'%(.+)%'
    match = re.search(pattern, content).group()
    new_line = match.replace('%', '').replace('template', subid)
    new_line += '\n'
    if subid not in content:
        newc = content.replace(match, new_line + match)
    else:
        newc = content
    return newc


def change_template(folder, subid):
    file_p = os.path.join(folder, subid, 'newsubject.tex')
    with open(file_p, 'r') as fid:
        sect = fid.read()
    sect = sect.replace('XXXXX', subid)
    with open(file_p, 'w') as fid:
        fid.write(sect)
    return None


def copy_template(folder, subid):
    folder_list = os.listdir('report_document')
    if subid not in folder_list:
        command = 'cp -r report_document/template report_document/{0}'.format(subid)
        os.system(command)
    else:
        print('The folder {0} exists yet.\n'.format(subid))
    command = 'cp {0}/*.png report_document/{1}/Figures'.format(folder, subid)
    os.system(command)
    return None


filesproc = pd.read_csv('dirsuccess.txt', header=None, sep=',')
filesproc.columns = ["directory",
                     "Subjectnumber"]
content, fmain = load_main()
for n in filesproc.index:
    row = filesproc.iloc[n]
    folder = row.directory
    subject_id = folder.split('\\')[-1].replace('-', '')[:-3]
    subject_id = 's' + subject_id
    copy_template(folder, subject_id)
    change_template(fmain[:-9], subject_id)
    content = change_content_main(content, subject_id)
with open(fmain, 'w') as fid:
    fid.write(content)
