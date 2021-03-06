//
//  main.cpp
//  data_process
//
//  Created by GoatWu on 2020/5/12.
//  Copyright © 2020 吴朱冠宇. All rights reserved.
//

#include <bits/stdc++.h>
#include "data_process.hpp"
#include "classifier.hpp"

using namespace std;

DataFrame data;
BPClassifier bp_clf(50,0.3,0.1); // (epoch, learning_rate_w, learning_rate_b)
void show(DataFrame &dt, BPClassifier &bp) {
    vector<int> ans = bp.predict(dt.X_test);
    vector<string> res = dt.tostring(ans);
    int num = 0;
    cout << "The result on the test dataset:\n";
    for (int i = 0; i < ans.size(); i++) {

        cout << "test " << setw(2) << right << i + 1 << ": ";
        cout << "result: " << setw(17) << left << res[i] << "  ";
        cout << "answer: " << setw(17) << left << dt.ans(i) << "\n";
        if (res[i] == dt.ans(i)) {
            num++;
        }
    }
    cout << num << " correct of " << ans.size() << " tests\n";
    cout << "accuracy: " << 1.0 * num / ans.size() << endl;
}

int main()
{
    srand(unsigned(time(0)));
    cout << "Hello Test!!!\n";
    data.read_file("iris.data");
    data.init();

    bp_clf.optimize.learning_rate_decay(0.00001);
    bp_clf.optimize.regularization("L2",0.00001);
    bp_clf.optimize.optimizer_adam();

    bp_clf.fit(data.X_train, data.Y_train);

    show(data, bp_clf);
    return 0;
}
