//
//  classifier2.cpp
//  data_process
//
//  Created by GoatWu on 2020/5/13.
//  Copyright © 2020 吴朱冠宇. All rights reserved.
//

#include "classifier.hpp"
#include "data_process.hpp"

BPClassifier::BPClassifier(int iters, double eta_w, double eta_b)
    : iters(iters), eta_w(eta_w), eta_b(eta_b) {}

BPClassifier::~BPClassifier() {}

void BPClassifier::get_num(const Matrix &X, const Matrix &Y)
{
    in_num = X.c;                                    // number of features
    ou_num = Y.c;                                    // number of label
    hd_num = min(NUM, (int)sqrt(in_num+ou_num)+6);   // a = 6
    return;
}

void BPClassifier::generate_array(double ***a, int n0, int n1)
{
    *a = new double *[n0];
    for(int i = 0; i < n0; i++)
    {
        (*a)[i] = new double [n1];
    }
    return;
}

void BPClassifier::generate_array(double ****a, int n0, int n1, int n2)
{
    *a = new double **[n0];
    for(int i = 0; i < n0; i++)
    {
        (*a)[i] = new double *[n1];
        for(int j = 0; j < n1; j++)
        {
            (*a)[i][j] = new double [n2];
        }
    }
    return;
}

void BPClassifier::random_start()
{
    srand((unsigned int) time(0));
    for(int i = 1; i < LAYER; i++)                         // input layer b = 0
        for(int j = 0; j < NUM; j++) b[i][j] = random_01();

    for(int i = 0; i < LAYER-1; i++)
        for(int j = 0; j < NUM; j++)
            for(int k = 0; k < NUM; k++) w[i][j][k] = random_01();
}

void BPClassifier::initialize_network(int flag)
{
    generate_array(&w, LAYER, NUM, NUM);
    generate_array(&b, LAYER, NUM);
    generate_array(&s, LAYER, NUM);
    if(!flag) return;
    generate_array(&delta, LAYER, NUM);
    return;
}

void BPClassifier::forward_propagation()
{
    for(int j = 0; j < hd_num; j++)       // calculate s for hidden layer
    {
        double tmp = 0;
        for(int i = 0; i < in_num; i++) tmp += w[0][i][j] * s[0][i];
        s[1][j] = sigmoid(tmp + b[1][j]);
    }

    for(int j = 0; j < ou_num; j++)       // calculate s for output layer
    {
        double tmp = 0;
        for(int i = 0; i < hd_num; i++) tmp += w[1][i][j] * s[1][i];
        s[2][j] = sigmoid(tmp + b[2][j]);
    }
    return;
}

void BPClassifier::calculate_loss(double &loss, const vector<double> &y)
{
    for(int i=0; i<ou_num; i++)
    {
        loss += 100*1/2*pow((s[2][i] - y[i]),2);
    }
    return ;
}

void BPClassifier::calculate_delta(const vector<double> &y)
{
    for(int i = 0; i < ou_num; i++)       // calculate delta for output layer
    {
        delta[2][i] = (s[2][i] - y[i]) * s[2][i] * (1 - s[2][i]);
    }
    for(int i = 0; i < hd_num; i++)       // calculate delta for hidden layer
    {
        double tmp = 0;
        for(int j = 0; j < ou_num; j++) tmp += w[1][i][j] * delta[2][j];
        delta[1][i] = tmp;
    }
    return;
}

void BPClassifier::improve_network(int t)
{
    for(int j = 0; j < ou_num; j++)
    {
        for(int i = 0; i < hd_num; i++)   // update w between hd/ou layer
        {
            w[1][i][j] -= eta_w * delta[2][j] * s[1][i];
        }
        b[2][j] -= eta_b * delta[2][j];   // update b for output layer
    }

    for(int j = 0; j < hd_num; j++)
    {
        for(int i = 0; i < in_num; i++)   // update w between in/hd layer
        {
            w[0][i][j] -= eta_w * delta[1][j] * s[0][i];
        }
        b[1][j] -= eta_b * delta[1][j];   // update b for hidden layer
    }
    return;
}

void BPClassifier::initialize_adam(double ***vdw,double **vdb,double ***sdw,double **sdb)
{
    for(int i= 0; i<LAYER; i++)
    {
        for(int j = 0; j<NUM; j++)
        {
            for(int k = 0; k<NUM; k++)
            {
                vdw[i][k][j] = 0;
                sdw[i][k][j] = 0;
            }
            vdb[i][j] = 0;
            sdb[i][j] = 0;
        }
    }
    return ;
}

void BPClassifier::improve_network_adam(double ***vdw,double **vdb,double ***sdw,double **sdb)
{
    double db = 0,dw = 0;
    for(int j = 0; j < ou_num; j++)
    {
        for(int i = 0; i < hd_num; i++)
        {
            dw = delta[2][j] * s[1][i];
            vdw[1][i][j] = 0.9*vdw[1][i][j] + 0.1*dw;
            sdw[1][i][j] = 0.999*sdw[1][i][j] + 0.001*dw*dw;

        }
        db = delta[2][j];
        vdb[2][j] = 0.9*vdb[2][j] + 0.1*db;
        sdb[2][j] = 0.999*sdb[2][j] + 0.001*db*db;

    }

    for(int j = 0; j < hd_num; j++)
    {
        for(int i = 0; i < in_num; i++)
        {
            dw = delta[1][j] * s[0][i];
            vdw[0][i][j] = 0.9*vdw[0][i][j] + 0.1*dw;
            sdw[0][i][j] = 0.999*sdw[0][i][j] + 0.001*dw*dw;
        }
        db = delta[1][j];
        vdb[1][j] = 0.9*vdb[1][j] + 0.1*db;
        sdb[1][j] = 0.999*sdb[1][j] + 0.001*db*db;
    }
    return;
}
void BPClassifier::improve_network_with_adam(double ***vdw,double **vdb,double ***sdw,double **sdb,int t)
{
    for(int j = 0; j < ou_num; j++)
    {
        for(int i = 0; i < hd_num; i++)
        {
            vdw[1][i][j] /= (1-pow(0.9,t));
            sdw[1][i][j] /= (1-pow(0.999,t));
            w[1][i][j] -= eta_w * vdw[1][i][j] / (sqrt(sdw[1][i][j]) + 1e-8);
        }
        vdb[2][j] /= (1-pow(0.9,t));
        sdb[2][j] /= (1-pow(0.999,t));
        b[2][j] -= eta_b * vdb[2][j] / (sqrt(sdb[2][j]) + 1e-8);
    }

    for(int j = 0; j < hd_num; j++)
    {
        for(int i = 0; i < in_num; i++)
        {
            vdw[0][i][j] /= (1-pow(0.9,t));
            sdw[0][i][j] /= (1-pow(0.999,t));
            w[0][i][j] -= eta_w * vdw[0][i][j] / (sqrt(sdw[0][i][j]) + 1e-8);
        }
        vdb[1][j] /= (1-pow(0.9,t));
        sdb[1][j] /= (1-pow(0.999,t));
        b[1][j] -= eta_b * vdb[1][j] / (sqrt(sdb[1][j]) + 1e-8);
    }
    return;
}

void BPClassifier::improve_network_L2()
{
    for(int j = 0; j < ou_num; j++)
    {
        for(int i = 0; i < hd_num; i++)
        {
            //here 150 should be replaced by the number of train examples
            w[1][i][j] *= (1-optimize.weight_decay*eta_w/150);
        }
    }

    for(int j = 0; j < hd_num; j++)
    {
        for(int i = 0; i < in_num; i++)
        {
            w[0][i][j] *= (1-optimize.weight_decay*eta_w/150);
        }
    }
    return;
}

void BPClassifier::backward_propagation(const vector<double> &y, int t, int iter,double &loss)
{
    calculate_delta(y);                   // y is grounding result
    calculate_loss(loss, y);
    if(optimize.isL2) improve_network_L2();
    if(!optimize.isadam) improve_network(t);
    return;
}

void BPClassifier::record_network(string name)
{
    ofstream myout;
    myout.clear();
    myout.open(name+"_network.txt", ios::out);

    myout << "input-layer-neurons-number: " << in_num << endl;
    myout << "hidden-layer-neurons-number: " << hd_num << endl;
    myout << "output-layer-neurons-number: " << ou_num << endl;

    myout << "weight-between-input-layer-and-hidden-layer:\n";
    for(int i = 0; i < in_num; i++)
        for(int j = 0; j < hd_num; j++)
        {
            myout << fixed << setprecision(20) << w[0][i][j] << (j == hd_num-1 ? "\n":"\t");
        }

    myout << "weight-between-hidden-layer-and-output-layer:\n";
    for(int i = 0; i < hd_num; i++)
        for(int j = 0; j < ou_num; j++)
        {
            myout << fixed << setprecision(20) << w[1][i][j] << (j == ou_num-1 ? "\n":"\t");
        }
    myout << "bias-for-hidden-layer:\n";
    for(int j = 0; j < hd_num; j++)
    {
        myout << fixed << setprecision(20) << b[1][j] << "\t";
    }

    myout << "\nbias-for-output-layer:\n";
    for(int j = 0; j < ou_num; j++)
    {
        myout << fixed << setprecision(20) << b[2][j] << "\t";
    }

    myout.close();
    return;
}

void BPClassifier::read_network(string name)
{
    ifstream myin;
    myin.clear();
    myin.open(name+"_network.txt", ios::in);

    string xyz;
    myin >> xyz >> xyz >> xyz >> xyz >> xyz >> xyz;

    myin >> xyz;
    for(int i = 0; i < in_num; i++)
        for(int j = 0; j < hd_num; j++) myin >> w[0][i][j];

    myin >> xyz;
    for(int i = 0; i < hd_num; i++)
        for(int j = 0; j < ou_num; j++) myin >> w[1][i][j];

    myin >> xyz;
    for(int j = 0; j < hd_num; j++) myin >> b[1][j];

    myin >> xyz;
    for(int j = 0; j < ou_num; j++) myin >> b[2][j];

    myin.close();
    return;
}

void BPClassifier::free_array()
{
    delete w;
    delete b;
    delete s;
    return;
}

int BPClassifier::forecast(const vector<double> &x)
{
    for(int i = 0; i < in_num; i++) s[0][i] = x[i];
    forward_propagation();
    double tmp = 0;                       // largest score
    int res = -1;                         // the kind of largest score
    for(int i = 0; i < ou_num; i++)
    {
        if(s[2][i] > tmp) tmp = s[2][res = i];
    }
    return res;
}

double BPClassifier::random_01()
{
    return (double) (rand()%1001) / 1000.0;
}

double BPClassifier::sigmoid(double x)
{
    return 1 / ( 1 + exp(-x) );
}

double BPClassifier::calculate_accuracy(const Matrix &X, const Matrix &Y, vector<int> v)
{
    int correct = 0;                      // number of the correct forecast
    for(int i : v)
    {
        int p = forecast(X.v[i]);
        correct += Y.v[i][p] > 0.99;
    }
    return 1.0 * correct / v.size();         // return accuracy
}

void BPClassifier::fit(const Matrix &X, const Matrix &Y)
{
    cout << string(70, '*')+"\n";
    cout << "Start to train the BP Neutual Network Classifier. \n";
    get_num(X, Y);
    initialize_network(1);

    int num = X.r-X.r/4;
    vector<int> id, id2;

    double loss = 0;
    optimize.learning_rate_record(eta_b,eta_w);

    double all_best = 0;
    for(int model = 0; model < 5; model++)
    {
        id.resize(0);
        id2.resize(0);
        for(int i = 0; i < X.r; i++) id.push_back(i);
        random_shuffle(id.begin(), id.end());        // instance in random order
        for (int i = num; i < X.r; i++) id2.push_back(id[i]);
        id.resize(num);

        double now_best = 0;
        random_start();
        double ***vdw, **vdb, ***sdw, **sdb;
        generate_array(&vdb,LAYER,NUM);
        generate_array(&vdw,LAYER,NUM,NUM);
        generate_array(&sdb,LAYER,NUM);
        generate_array(&sdw,LAYER,NUM,NUM);


        for(int iter = 0; iter < iters; iter++)
        {
            loss = 0;
            if(optimize.isadam) initialize_adam(vdw,vdb,sdw,sdb);

            for(int p : id)                              // trained by every instance
            {
                for(int i = 0; i < in_num; i++) s[0][i] = X.v[p][i];
                forward_propagation();
                backward_propagation(Y.v[p], p+iter*num, iter, loss);

                if(optimize.isadam) improve_network_adam(vdw,vdb,sdw,sdb);
            }
            cout << "epoch "<< iter+1 << " ----->loss:" << loss << endl;

            optimize.learning_rate_decay(eta_b,eta_w,iter+1);

            if(optimize.isadam) improve_network_with_adam(vdw,vdb,sdw,sdb,iter+1);

            double tmp = calculate_accuracy(X, Y, id);
            if(now_best < tmp)
            {
                record_network("now_best");              // store the best network
                now_best = tmp;
                double tmpp = calculate_accuracy(X, Y, id2);
                if(now_best > all_best )       //tmpp > 0.90
                {
                    record_network("all_best");
                    all_best = now_best;
                }
            }

           //if(now_best-tmp >= 3e-2) break;              // early stopping

        }

        cout << "Finish training model " << model+1;
        cout << ". Accuracy on training set: " << now_best << endl;
    }
    for(int i = 0; i < X.r; i++) id.push_back(i);
    read_network("all_best");                            // read the best network
    cout << "End training Classifier!\n";
    cout << "Final Accuracy on train set: " << calculate_accuracy(X, Y, id);
    cout << "\n"+string(70, '*')+"\n";

    free_array();
    return;
}

vector<int> BPClassifier::predict(const Matrix &X)
{
    initialize_network(0);
    read_network("all_best");
    vector<int> res;
    for(int i = 0; i < X.r; i++) res.push_back(forecast(X.v[i]));
    free_array();
    return res;
}


