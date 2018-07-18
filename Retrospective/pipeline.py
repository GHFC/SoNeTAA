#!/usr/bin/env python
# coding=utf-8

# ==============================================================================
# title : pipeline.py
# description : Import EEG files in MNE Python & extract alpha infos
# input: EDF+ files
# output: results summary in CSV file + figure for each participant
# author : G. Dumas, Human Genetics & Cognitive Functions, Institut Pasteur
# date : 2018
# version : 2.0
# usage : python pipeline.py
# python_version : 3.6
# license : BSD (3-clause)
# ==============================================================================

import os
import os.path as op
import mne
import numpy as np
import matplotlib
from copy import deepcopy
import matplotlib.pyplot as plt
from scipy.stats import kurtosis
from mne.time_frequency import psd_welch
from scipy.stats import ttest_ind

# Initialisation
matplotlib.use('Agg')
np.random.seed(42)
mne.set_log_level(verbose='CRITICAL')

# Parameters
subjectsNum = 135
inputFolder = 'raw'
outputFolder = 'results'

if not os.path.exists(outputFolder):
    os.mkdir(outputFolder)

# Header of the CSV file summarizing all the results
with open(os.path.join(outputFolder, 'Python_results.csv'), 'w') as outcsv:
    outcsv.write("Subject,Session,Group,BadChannels,EC,EO,AlphaChannel,AlphaFreq,AlphaPeak\n")

# Loop over groups
for subjectType in ['', 'T']:

    # Loop over participants
    for subjectNum in range(subjectsNum + 1):
        input_fname_mask = 'EEG-%s%03d' % \
                           (subjectType, subjectNum + 1)
        f_names = [f for f in os.listdir(inputFolder) if input_fname_mask in f]
        nsession = len(f_names)

        # Loop over sessions
        for f_name in f_names:
            print('\n', f_name)
            with open(os.path.join(outputFolder, 'Python_results.csv'), 'a') as outcsv:
                outcsv.write("{},{},{}".format(subjectNum + 1,
                                               int(f_name[f_name.find('_') + 1:-4]),
                                               subjectType))
            data = mne.io.read_raw_edf(op.join(inputFolder, f_name),
                                       montage=None, eog=None, misc=None,
                                       stim_channel=-1, annot=None,
                                       annotmap=None, preload=True,
                                       verbose=False)
            output_fname = deepcopy(f_name)

            # Handle channels
            for ich, lch in enumerate(data.info['ch_names']):
                newlabel = lch.replace('EEG ', '').replace('-Ref', '')
                data.info['ch_names'][ich] = newlabel
                data.info['chs'][ich]['ch_name'] = newlabel

            for ch in [u'POL E', u'POL PG1', u'POL PG2', u'POL T1',
                       u'POL T2', u'POL X1', u'POL X2', u'POL X3',
                       u'POL X4', u'POL X5', u'POL X6', u'POL X7']:
                try:
                    data.set_channel_types({ch: 'misc'})
                except:
                    print(ch, "is absent!")

            # Setup 10/20 montage
            montage = mne.channels.read_montage('standard_1020')
            data.set_montage(montage)

            # Extract the bad or missing channels
            pick_chans = [p for p in data.ch_names if
                          ('POL' not in p) and ('STI' not in p) and
                          ('A1' not in p) and ('A2' not in p) and
                          (p not in data.info['bads'])]
            data.drop_channels([p for p in data.ch_names
                                if ('POL' in p) or ('STI' in p) or
                                   ('A1' in p) or ('A2' in p) or
                                   (p in data.info['bads'])])

            # Filtering
            data.filter(phase='zero',
                        fir_window='hamming',
                        fir_design='firwin',
                        l_freq=None, h_freq=48,
                        l_trans_bandwidth='auto',
                        h_trans_bandwidth='auto',
                        filter_length='auto')

            # Plot SD & Kurtosis
            SD = data._data.std(axis=1)
            K = kurtosis(data._data.T)

            fname = op.join(outputFolder,
                            output_fname.replace('.edf', '_topo.pdf'))
            fig = plt.figure()
            plt.subplot(1, 2, 1)
            mne.viz.plot_topomap(SD, data.info, show=False,
                                 show_names=True, names=pick_chans)
            plt.title('Standard Deviation')
            plt.subplot(1, 2, 2)
            mne.viz.plot_topomap(K, data.info, show=False,
                                 show_names=True, names=pick_chans)
            plt.title('Kurtosis')
            plt.tight_layout()
            plt.savefig(fname)

            # Detect bad channels
            mask = SD > 0
            change = True
            while change:
                newmask = mask
                outlierSD = SD > (SD[mask].mean() + 2 * SD[mask].std())
                outlierK = K > (K[mask].mean() + 2 * K[mask].std())
                newmask[outlierSD] = False
                newmask[outlierK] = False
                if (newmask != mask).any():
                    mask[outlierSD] = False
                    mask[outlierK] = False
                    change = True
                else:
                    change = False
            data.info['bads'] += [ch for i, ch in enumerate(data.ch_names)
                                  if not mask[i]]

            # Consolidate the bad channels
            pick_chans = [p for p in data.ch_names if
                          ('POL' not in p) and ('STI' not in p) and
                          ('A1' not in p) and ('A2' not in p) and
                          (p not in data.info['bads'])]
            drop_chans = [p for p in data.ch_names
                          if ('POL' in p) or ('STI' in p) or
                             ('A1' in p) or ('A2' in p) or
                             (p in data.info['bads'])]
            print("Dropped channels:")
            for ch in drop_chans:
                print(ch)
            data.drop_channels(drop_chans)
            with open(os.path.join(outputFolder, 'Python_results.csv'), 'a') as outcsv:
                outcsv.write(',' + str(len(drop_chans)))

            # Extract events
            events = data._raw_extras[0]['events']
            startRec = 0
            stopRec = events[-1][0]
            unknowns = []

            # Detect eyes open and eyes closed intervals
            eyesOpened = 0
            eyesClosed = 0
            EO = []
            EC = []
            STOP = False
            for ievt, levt in enumerate(events):
                label = levt[2].lower()
                if label[0] != '+' and label[0] != '*':
                    unknown = True
                    if 'y' in label and not(STOP):
                        if 'o' in label:
                            # print("Eyes open at", levt[0])
                            if eyesClosed > 0 and levt[0] - eyesClosed > 1.:
                                EC.append([eyesClosed, levt[0]])
                            eyesOpened = levt[0]
                        if 'f' in label and not(STOP):
                            # print("Eyes close at", levt[0])
                            if eyesOpened > 0 and levt[0] - eyesOpened > 1.:
                                EO.append([eyesOpened, levt[0]])
                            eyesClosed = levt[0]
                        unknown = False

            # Perform spectral analyses
            fmin, fmax = 2, 48  # look at frequencies between 2 and 48Hz
            n_fft = 1024  # the FFT size (n_fft). Ideally a power of 2

            tmin, tmax = startRec, stopRec
            picks = [i for i, ch in enumerate(data.ch_names)
                     if 'o' in ch[0].lower() or 'p' in ch[0].lower()]
            channels = [ch for i, ch in enumerate(data.ch_names)
                        if 'o' in ch[0].lower() or 'p' in ch[0].lower()]

            psds, freqs = psd_welch(data, n_fft=1024, n_overlap=512,
                                    tmin=tmin, tmax=tmax,
                                    fmin=fmin, fmax=fmax,
                                    proj=False, picks=picks,
                                    n_jobs=1, verbose=False)

            psds = 10 * np.log10(psds)
            psds_mean = psds.mean(0)
            psds_std = psds.std(0)

            # Plot the global spectrum
            fname = op.join(outputFolder,
                            output_fname.replace('.edf', '_spectrum.pdf'))
            plt.figure()
            ax = plt.axes()
            ax.plot(freqs, psds_mean, color='k')
            ax.fill_between(freqs, psds_mean - psds_std,
                            psds_mean + psds_std,
                            color='k', alpha=.5)
            ax.set(title='Welch PSD', xlabel='Frequency',
                   ylabel='Power Spectral Density (dB)')
            plt.savefig(fname)

            def poweeg(data, tmin, tmax):
                """Extract Power Spectrum from EEG data and timing."""
                psds, freqs = psd_welch(data, n_fft=1024, n_overlap=512,
                                        tmin=tmin, tmax=tmax,
                                        fmin=fmin, fmax=fmax,
                                        proj=False, picks=picks,
                                        n_jobs=1, verbose=False)
                return psds, freqs

            # Compare the spectra for EO & EC
            EO_Global = np.zeros([len(EO), psds.shape[0], psds.shape[1]])
            goodEO = []
            for i, times in enumerate(EO):
                # print(i, times[0], times[1])
                try:
                    psds, freqs = poweeg(data, times[0], times[1])
                    EO_Global[i, :, :] = psds
                    goodEO.append(i)
                except:
                    print(times[1] - times[0])

            EC_Global = np.zeros([len(EC), psds.shape[0], psds.shape[1]])
            goodEC = []
            for i, times in enumerate(EC):
                # print(i, times[0], times[1])
                try:
                    psds, freqs = poweeg(data, times[0], times[1])
                    EC_Global[i, :, :] = psds
                    goodEC.append(i)
                except:
                    print(times[1] - times[0])

            print("EC length:", len(goodEC))
            print("EO length:", len(goodEO))
            with open(os.path.join(outputFolder, 'Python_results.csv'), 'a') as outcsv:
                outcsv.write(',' + str(len(goodEC)) +
                             ',' + str(len(goodEO)))

            [t, p] = ttest_ind(EC_Global[goodEC, :, :],
                               EO_Global[goodEO, :, :],
                               axis=0)

            # Search for the alpha peak
            mask = (freqs > 5) * (freqs < 20)
            try:
                tmp = t[:, mask]
                idx = np.argwhere(tmp == tmp.max())
                c_peak, f_peak, a_peak = channels[idx[0][0]], freqs[mask][idx[0][1]], tmp.max()
            except:
                c_peak, f_peak, a_peak = np.nan, np.nan, np.nan

            with open(os.path.join(outputFolder, 'Python_results.csv'), 'a') as outcsv:
                outcsv.write(',' + str(c_peak) +
                             ',' + str(f_peak) +
                             ',' + str(a_peak) +
                             '\n')

            # Plot EC vs. EO spectra for alpha peak detection
            fname = op.join(outputFolder,
                            output_fname.replace('.edf', '_EO-vs-EC.pdf'))

            if not(np.isnan(t).any()):
                plt.figure()
                plt.subplot(2, 1, 1)
                plt.plot(t.T)
                xt = range(0, len(freqs), 10)
                plt.xticks(xt, np.round(freqs[xt] * 10.) / 10.)
                plt.legend(channels)
                plt.title('EC vs. EO')
                plt.xlabel('Frequency')
                plt.ylabel('T-value')
                plt.subplot(2, 1, 2)
                plt.plot(-np.log(p.T))
                plt.xticks(xt, np.round(freqs[xt] * 10.) / 10.)
                plt.legend(channels)
                plt.xlabel('Frequency')
                plt.ylabel('-log(p-value)')
                index = np.argmax(np.max(-np.log(p)) == -np.log(p))
                ich, ifr = np.unravel_index(index, p.shape)
                plt.title('Maximum at ' + str(freqs[ifr]) +
                          'Hz for ' + channels[ich])
                plt.tight_layout()
                plt.savefig(fname)
            else:
                print("Error for contrast EC vs. EO")
