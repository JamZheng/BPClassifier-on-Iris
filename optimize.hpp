#ifndef data_process_hpp
#define data_process_hpp

#include <bits/stdc++.h>

using namespace std;

class Optimize
{
private:
    double decay_rate;
    void dropout(double keep_prop = 1);
    void L2(double weight_decay);
public:
    Optimize();

    bool isdropout,isL2;
    double weight_decay,keep_prop;

    void learning_rate_decay(double decay_rate);
    void learning_rate_decay(double &eta,int epoch_num);

    void regularization(string c,int parameter);
    //void optimizer_adam();
};

Optimize::Optimize():isdropout(false),isL2(false)
{
    decay_rate = 0;
    weight_decay = 0;
    keep_prop = 1;
}

void Optimize::dropout(double keep_prop)
{
    this->keep_prop = keep_prop;
    return ;
}
void Optimize::L2(double weight_decay)
{
    this->weight_decay = weight_decay;
    return ;
}


void Optimize::learning_rate_decay(double decay_rate)
{
    this->decay_rate = decay_rate;
    return ;
}
void Optimize::learning_rate_decay(double &eta,int epoch_num)
{
    eta = eta/(1+decay_rate*epoch_num);
    //eta = pow(0.95,epoch_num) * eta;
    return ;
}


void Optimize::regularization(string c, int parameter)
{
    if(c == "dropout")
    {
        isdropout = true;
        dropout(parameter);
    }
    if(c == "L2")
    {
        isL2 = true;
        L2(parameter);
    }
    return ;
}

#endif