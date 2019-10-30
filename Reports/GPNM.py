#!/usr/bin/env python
# coding=utf-8

# ==============================================================================
# title : GPNM.py
# description : Compute a gaussian process normative model based on age
# author : Guillaume Dumas, Human Genetics & Cognitive Functions, Institut Pasteur
# date : 2019-10-30
# version : 1.0
# usage : python GPNM.py
# python_version : 3.6
# license : BSD (3-clause)
# ============================================================================== 

import pandas as pd
import numpy as np
from sklearn.gaussian_process import GaussianProcessRegressor
from sklearn.gaussian_process.kernels import RBF, WhiteKernel, ConstantKernel as C
np.random.seed(42)

# Load phenotypes
# data = pd.read_csv(“phenotypes.csv”)
age = np.random.randint(1, 5000, 100)
ctr = np.random.rand(100)
data = pd.DataFrame(data={'age': age,
                          'ctr': ctr > 0.5,
                          'score': np.log(age) + np.random.randn(100) + ctr})

# Normalize the raw data globally
data.loc[:, 'score'] = (data.loc[:, 'score']-np.nanmean(data.loc[:, 'score'])) / np.nanstd(data.loc[:, 'score'])

# Check the range of Age and Score
min_age = np.min(data.loc[:, 'age'])
max_age = np.max(data.loc[:, 'age'])
min_score = np.min(data.loc[:, 'score'])
max_score = np.max(data.loc[:, 'score'])

# Define parameters of the kernel
kernel = RBF(length_scale=1.0, length_scale_bounds=(1e-10, 1e5)) \
    + WhiteKernel(noise_level=1, noise_level_bounds=(1e-10, 1e5))
gp = GaussianProcessRegressor(kernel=kernel)

# Select CTR to run the normative model 
sel = data.loc[(data.ctr == True), ['age', 'score']].values
d = sel[:, :1]

# Train the Gaussian Process
X = np.atleast_2d(data.loc[(data.ctr == True), 'age'].values).T
y = np.squeeze(data.loc[(data.ctr == True), 'score'].values.reshape(1, -1).T)
gp.fit(X[np.isfinite(y)], y[np.isfinite(y)])

# Compute the normative score
y_mean, y_std = gp.predict(np.atleast_2d(data.age.values).T, return_std=True)
data.loc[:, 'GPNM'] = (data.loc[:, 'score'].values - y_mean)/y_std

# Calculate ranks associated with the normative scores
data.loc[(data.GPNM <= -2), 'rank'] = -2
data.loc[(data.GPNM > -2)*(data.GPNM <= -1), 'rank'] = -1
data.loc[(data.GPNM > -1)*(data.GPNM <= +1), 'rank'] = 0
data.loc[(data.GPNM > +1)*(data.GPNM <= +2), 'rank'] = 1
data.loc[(data.GPNM > +2), 'rank'] = 2

data.to_csv("phenotypes_gpnm.csv")
