#!/usr/bin/env python2

from .util import get_logger
#from .data import Pokemon
from .config import config

import numpy as np
import pandas as pd
import xgboost as xgb

import matplotlib.pyplot as plt

logger = get_logger(__name__)


def normalize(d):
    d -= np.min(d, axis=0)
    d = 1.0 * d / np.max(d, axis=0)
    return d


def predict_and_visualize(data, model):
    for row in data:
        test = xgb.DMatrix(np.array([row[1:-1]]), label=np.array([row[-1]]))
        r = int(round(model.predict(test)))
        if not r == row[-1]:
            print row[0], r, row[-1], model.predict(test)[0]

    model.dump_model('dump.raw.txt')

    xgb.plot_importance(model)
    plt.savefig('importance.png')
    xgb.plot_tree(model)
    plt.savefig('decision_tree.png')


def training(data):
    num_train = 493
    train = data[:num_train, :]
    test = data[num_train:, :]

    train_X = train[:, 1:-1]
    #train_X = normalize(train_X)
    train_Y = train[:, -1]

    test_X = test[:, 1:-1]
    #test_X = normalize(test_X)
    test_Y = test[:, -1]

    xg_train = xgb.DMatrix(train_X, label=train_Y)
    xg_test = xgb.DMatrix(test_X, label=test_Y)

    param = {}
    #param['objective'] = 'binary:logistic'
    param['objective'] = 'reg:linear'
    param['max_depth'] = 5
    param['slient'] = 1
    param['eta'] = 1
    param['nthread'] = 4
    param['eval_metric'] = ['auc', 'ams@0']

    watchlist = [(xg_train, 'train'), (xg_test, 'test')]
    num_round = 5
    bst = xgb.train(param, xg_train, num_round, watchlist)

    pred = np.round(bst.predict(xg_test))
    error_rate = 1.0 * np.sum(pred != test_Y) / test_Y.shape[0]
    print('Test error using {} = {}'.format(param['objective'], error_rate))

    return bst


def main():
    logger.info('start')
    df = pd.read_csv(config['data_path'], sep=',')
    data_df = pd.DataFrame(
        df, columns=['Name', 'Height_m', 'Weight_kg', 'hasGender', 'Catch_Rate', 'Total', 'isLegendary'])
    data_df['isLegendary'] = data_df.isLegendary.apply(int)
    data_df['hasGender'] = data_df.hasGender.apply(int)
    data = data_df.values
    model = training(data)
    predict_and_visualize(data, model)
    logger.info('end')


if __name__ == '__main__':
    main()
