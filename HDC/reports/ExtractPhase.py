"""Plot trajectories from a given HKB files."""

import numpy as np
import pandas as pd
import scipy.signal as sg
from scipy import stats
import matplotlib.pyplot as plt
import os
import re


def get_meanscores(dfres, nfolder, subjectid):
    """Get mean of the scores."""
    scores = ['ToM', 'Coordination', 'Tache', 'Intentionalite']
    dres = pd.DataFrame(columns=['directory'] +
                        scores + ['task', 'coop', 'SubjectId'])
    task_op = ['1', '2']
    vp_behavior = [0, 1]
    result = dict()
    for tas in task_op:
        dft = dfres.loc[dfres.task == tas, :]
        for beh in vp_behavior:
            dfb = dft.loc[dft.coop == beh, :]
            av = dfb.mean()
            for score in scores:
                result.update(dict(zip([score], [av.loc[score]])))
            result.update(dict(zip(['task', 'coop'], [int(tas), beh])))
            result.update(dict(zip(['directory'], [nfolder])))
            result.update(dict(zip(['SubjectId'], [subjectid])))
            dres = dres.append(result, ignore_index=True)
    return dres


def save_meanscores(dfres):
    """Save the data to csv file."""
    try:
        fid = open('resume.txt', 'r')
        fid.close()
        dfres.to_csv('resume.txt', mode='a', header=False, index=False)
    except FileNotFoundError:
        dfres.to_csv('resume.txt', index=False)


def plot_scores(participant, folder, cohort):
    """Plot all the scores for the participants."""
    colors = ['blue', 'red', 'green', 'orange']
    scores = ['Coordination', 'Tache', 'ToM', 'Intentionalite']
    plt.figure(figsize=(12, 9))
    for index, score in enumerate(scores):
        plt.subplot(len(scores), 5, 1 + 5*index)
        y1, x, _ = plt.hist(x=participant.loc[:, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color=colors[index],
                            histtype='stepfilled'
                            )
        y2, x, _ = plt.hist(x=cohort.loc[:, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color='black',
                            linewidth=3,
                            histtype='step'
                            )
        plt.plot(cohort.loc[:, score].mean() * np.ones(2),
                 [0, max(y1.max(), y2.max())+1],
                 color=colors[index],
                 linewidth=1)
        plt.xlim([0, 1])
        plt.ylim([0, max(y1.max(), y2.max())+1])
        plt.yticks([])
        plt.ylabel(score, size=14)
        if index == 0:
            plt.title('Global')
    
        plt.subplot(len(scores), 5, 2 + 5*index)
        y1, x, _ = plt.hist(x=participant.loc[participant.coop == 0, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color=colors[index],
                            histtype='stepfilled'
                            )
        y2, x, _ = plt.hist(x=cohort.loc[cohort.coop == 0, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color='black',
                            linewidth=3,
                            histtype='step'
                            )
        plt.plot(cohort.loc[cohort.coop == 0, score].mean() * np.ones(2),
                 [0, max(y1.max(), y2.max())+1],
                 color=colors[index],
                 linewidth=1)
        plt.xlim([0, 1])
        plt.ylim([0, max(y1.max(), y2.max())+1])
        plt.yticks([])
        if index == 0:
            plt.title('VP Competitif')
    
        plt.subplot(len(scores), 5, 3 + 5*index)
        y1, x, _ = plt.hist(x=participant.loc[participant.coop == 1, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color=colors[index],
                            histtype='stepfilled'
                            )
        y2, x, _ = plt.hist(x=cohort.loc[cohort.coop == 1, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color='black',
                            linewidth=3,
                            histtype='step'
                            )
        plt.plot(cohort.loc[cohort.coop == 1, score].mean() * np.ones(2),
                 [0, max(y1.max(), y2.max())+1],
                 color=colors[index],
                 linewidth=1)
        plt.xlim([0, 1])
        plt.ylim([0, max(y1.max(), y2.max())+1])
        plt.yticks([])
        if index == 0:
            plt.title('VP Cooperatif')
    
        plt.subplot(len(scores), 5, 4 + 5*index)
        y1, x, _ = plt.hist(x=participant.loc[participant.task == 1, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color=colors[index],
                            histtype='stepfilled'
                            )
        y2, x, _ = plt.hist(x=cohort.loc[cohort.task == 1, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color='black',
                            linewidth=3,
                            histtype='step'
                            )
        plt.plot(cohort.loc[cohort.task == 1, score].mean() * np.ones(2),
                 [0, max(y1.max(), y2.max())+1],
                 color=colors[index],
                 linewidth=1)
        plt.xlim([0, 1])
        plt.ylim([0, max(y1.max(), y2.max())+1])
        plt.yticks([])
        if index == 0:
            plt.title('Tache: in-phase')
    
        plt.subplot(len(scores), 5, 5 + 5*index)
        y1, x, _ = plt.hist(x=participant.loc[participant.task == 2, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color=colors[index],
                            histtype='stepfilled'
                            )
        y2, x, _ = plt.hist(x=cohort.loc[cohort.task == 2, score],
                            bins=20,
                            range=[0, 1],
                            normed=True,
                            color='black',
                            linewidth=3,
                            histtype='step'
                            )
        plt.plot(cohort.loc[cohort.task == 2, score].mean() * np.ones(2),
                 [0, max(y1.max(), y2.max())+1],
                 color=colors[index],
                 linewidth=1)
        plt.xlim([0, 1])
        plt.ylim([0, max(y1.max(), y2.max())+1])
        plt.yticks([])
        if index == 0:
            plt.title('Tache: anti-phase')
    plt.tight_layout()
    plt.savefig(os.path.join(folder, 'resume.png'))
    # plt.show()


def get_score(data, goal):
    """Get the score for the trial.

    parameters
    --------------------------
        data: str path file
    --------------------------
    output
        score: float result.
    """
    import pandas as pd
    metronomeL = 3000
    transcient1L = 1000
    behaviorL = 4096
    lims = [metronomeL + transcient1L,
            metronomeL + transcient1L + (behaviorL * 2)]
    try:
        df = pd.read_csv(data, header=None, sep='\t')
    except:
        return None
    df.columns = ["time",
                  "vp_position",
                  "vp_speed",
                  "human_position",
                  "human_speed",
                  "hkb_coupling"]
    if df.loc[0].time > 0:
        df.drop(df.index[0], inplace=True)
    if df.time.iloc[-1] < lims[1]:
        return None
    lims[0] = np.argmax(df.time > lims[0])
    lims[1] = np.argmax(df.time > lims[1])

    X = sg.hilbert(df.vp_position)
    vp_phase = np.unwrap(np.angle(X[lims[0]:lims[1]]))
    vp_amp = np.abs(X[lims[0]:lims[1]])

    X = sg.hilbert(df.human_position)
    human_phase = np.unwrap(np.angle(X[lims[0]:lims[1]]))
    human_amp = np.abs(X[lims[0]:lims[1]])

    difampli = np.mean(human_amp - vp_amp)
    score = np.abs(np.mean(np.exp(1.j * (human_phase - vp_phase))))
    dphi = human_phase - vp_phase
    task_score = 1 - np.mean(np.abs(np.angle(
                                    np.exp(1.j * (dphi - goal))) / np.pi))
    # plt.plot(np.abs(np.angle(np.exp(1.j*(dphi-goal)))/np.pi))

    return score, task_score, difampli


def atoi(text):
    """Nothing."""
    return int(text) if text.isdigit() else text


def natural_keys(text):
    """Alist.sort(key=natural_keys) sorts in human order.

    http://nedbatchelder.com/blog/200712/human_sorting.html
    (See Toothy's implementation in the comments)
    """
    return [atoi(c) for c in re.split('(\d+)', text)]


def int_score(parameters):
    """Vp behavior = 1 if is cooperative and 0 if is competitive."""
    mu = parameters['Mu']
    task = parameters['Task']
    cooperation = parameters['Cooperation']
    if (mu == '1') and (task == '1'):
        # cooperative
        vp_behavior = 1
    elif mu == '1' and task == '2':
        # competitive
        vp_behavior = 0
    elif mu == '-1' and task == '1':
        # competitive
        vp_behavior = 0
    elif mu == '-1' and task == '2':
        # cooperative
        vp_behavior = 1
    intention_score = 1 - np.abs(vp_behavior - float(cooperation))
    return intention_score, vp_behavior


def get_goal(parameters):
    """Goal depends on task.

    if task is 1 inphase dphi -> 0
    task is 2 antiphase dphi -> pi
    """
    task = parameters['Task']
    if task == '1':
        goal = 0
    elif task == '2':
        goal = np.pi
    return goal


def get_parameters(filedata, fields):
    """Get the parameters for the trial.

    input
        filedata: path of the report file str
        fields: str list with the parameters to load
    output
        list with the parameters
    """
    from pandas import DataFrame
    df = DataFrame.from_csv(filedata, header=None, sep='=')
    df.columns = ['value']
    try:
        assert(set(fields).issubset(df.index)), "Non completed Trial"
    except AssertionError:
        return None
    parameters = list()
    for field in fields:
        parameters.append(df.loc[field]['value'])
    return parameters


def check_stability(participant, folder):
    """Plot the stability."""
    scores = ['Coordination', 'Tache', 'ToM', 'Intentionalite']
    colors = ['blue', 'red', 'green', 'orange']
    fig, axes = plt.subplots(2, 2, figsize=(14, 7))
    axes = axes.reshape(4, )
    fig2, axes2 = plt.subplots()
    bins = np.arange(0, participant.index[-1], 1)
    diffelist = list()
    for index, score in enumerate(scores):
        mea = np.zeros(len(bins))
        std = np.zeros(len(bins))
        for idx, num in enumerate(bins):
            mea[idx] = participant.loc[:num, score].mean()
            std[idx] = participant.loc[:num, score].std()
        axes[index].plot(bins, mea, '-', color=colors[index])
        axes[index].plot(bins, std, '--', color=colors[index])
        axes[index].legend(['Moyenne', 'Deviation Standard'])
        axes[index].set_title(score)
        axes[index].set_xlim([bins[0], bins[-1]])
        axes[index].set_xlabel('Nombre d\'essais pris en compte')
        axes[index].set_ylim([0, 1])
        diffeabs = np.abs(np.diff(mea[1::5]))
        diffelist.append(diffeabs)
        axes2.plot(diffeabs, '--', color=colors[index])
    confidence = 0.95
    diffe = np.asarray(diffelist)
    means = diffe.mean(0)
    serror = stats.sem(diffe)
    h = serror * stats.t._ppf((1 + confidence) / 2, diffe.shape[0])
    axes2.errorbar(np.arange(means.shape[0]), means, h, fmt='ok', lw=3)
    axes2.hlines(xmin=0, xmax=means.shape[0],
                 y=diffe.mean(),
                 linewidth=2.0,
                 color="red")

    # axes2.errorbar(bins[1:] - 1, means, [means - mins, maxes - means],
    #               fmt='.k', ecolor='gray', lw=1)

    axes2.set_xlim([0, means.shape[0]])
    axes2.set_title('Confidence interval')
    axes2.set_xlabel('Nombre d\'essais pris en compte')
    axes2.legend(scores)
    fig.tight_layout()
    fig.savefig(os.path.join(folder, 'stability.png'))
    fig2.tight_layout()
    fig2.savefig(os.path.join(folder, 'confidence.png'))
    # fig2.show()


rootdir = os.path.dirname(os.path.abspath(__file__))
stock_list = list(f[0] for f in os.walk(rootdir) if 'dir' in f[0])
try:
    dirsuccess = open('dirsuccess.txt', 'r')
    lines = dirsuccess.read()
    firstrun = False
    dirsuccess.close()
    dirsuccess = open('dirsuccess.txt', 'a')
except FileNotFoundError:
    dirsuccess = open('dirsuccess.txt', 'a')
    firstrun = True
fields = ['Task', 'Mu', 'Humanness', 'Cooperation']
for nf, folder in enumerate(stock_list):
    if firstrun:
        dirsuccess.writelines(folder + ',' + str(nf + 1) + '\n')
        subjectid = 1 + nf
    elif folder in lines:
        continue
    else:
        subjectid = int(lines.split('\n')[-2].split(',')[1].replace('\n', ''))
        subjectid += 1
        print(subjectid)
        dirsuccess.writelines(folder + ',' + str(subjectid) + '\n')
    print("Reading folder:\n{0}".format(folder))
    files = list(f for f in os.listdir(folder))
    files.sort(key=natural_keys)
    parameters = dict(zip(fields, [None] * 4))
    dff = pd.DataFrame(columns=fields + ['n_trial'])
    good = True
    for n, filee in enumerate(files):
        if 'data' in filee:
            filepathdata = os.path.join(folder, filee)
            filepathreport = os.path.join(folder, files[n + 1])
            parameters.update(n_trial=natural_keys(filee)[1])
            try:
                parameters.update(zip(fields,
                                      get_parameters(filepathreport, fields)))
                parameters['Humanness'] = float(parameters['Humanness'])
            except TypeError:
                print('Trial {0} discarted'.format(natural_keys(filee)[1]))
                continue
            parameters.update(dict(goal=get_goal(parameters)))

            result = get_score(filepathdata, parameters['goal'])
            if result is None:
                print('Trial {0} discarted'.format(natural_keys(filee)[1]))
                good = False
            else:
                parameters.update(zip(['Coordination',
                                       'Tache',
                                       'amplitud_dif_mean'],
                                      result))
            if good is True:
                parameters.update(dict(zip(['ToM', 'vp_behavior'],
                                           int_score(parameters))))
                dff = dff.append(parameters, ignore_index=True)
            else:
                print('Incompleted Trial data')
            good = True
        else:
            continue
    humanness_average = np.mean(dff.Humanness)
    humanness_std = np.std(dff.Humanness)
    dff.rename(columns={'Humanness': 'Intentionalite',
                        'Task': 'task', 'vp_behavior': 'coop'}, inplace=True)
    check_stability(dff, folder)
    dff.to_csv(os.path.join(folder, 'parameters.txt'))
    dfmean = get_meanscores(dff, folder[-19:-3].replace('-', ''), subjectid)
    save_meanscores(dfmean)
dirsuccess.close()
cohort = pd.read_csv('resume.txt', sep=',')
n_exp = np.unique(cohort.SubjectId)
filesproc = pd.read_csv('dirsuccess.txt', header=None, sep=',')
filesproc.columns = ["directory",
                     "SubjectId"]
for n in n_exp:
    cohortn = cohort.loc[cohort.SubjectId != n]
    folder = filesproc.loc[filesproc.SubjectId == n].directory
    participant = pd.read_csv(os.path.join(folder.values[0],
                                           'parameters.txt'),
                              index_col=0)
    plot_scores(participant, folder.values[0], cohortn)
