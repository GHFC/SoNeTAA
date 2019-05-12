"""Project Generator."""

import numpy as np
import numpy.matlib as nm

strengths = [-3]
couplings = [1, -1]
#transitions = [0,1]
transitions = [0]
tasks = [1, 2]
repetitions = 10
nTrials = (len(strengths) *
           len(couplings) *
           len(transitions) *
           len(tasks)) * repetitions
trials = []
for s in strengths:
    for c in couplings:
        for t in transitions:
            for ta in tasks:
                for r in range(repetitions):
                    trials.append([c, t, s, ta])

np.random.shuffle(trials)

blocksize = 10
if np.random.uniform() > 0.5:
    blocks = nm.repmat(tasks, 1, nTrials / (2 * blocksize))
else:
    blocks = nm.repmat(np.flipud(tasks), 1, nTrials / (2 * blocksize))
bin = 0
blocks = blocks[0]
for block in blocks:
    for i in range(blocksize):
        trials[i + (blocksize * bin)][3] = block
    bin += 1

with open('trials.xml', 'w') as fid:
    for nt, trial in enumerate(trials):
        cad1 = '<trial{0}><mu>{1}</mu>'.format(nt+1, trial[0])
        cad2 = '<tr>{0}</tr><a>{1}</a>'.format(trial[1], trial[2])
        cad3 = '<task>{0}</task></trial{1}>\n'.format(trial[3], nt+1)
        xmlst = cad1 + cad2 + cad3
        fid.write(xmlst)
print("File 'trials.xml' created correctly")
print("launching HDC...")
