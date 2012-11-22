#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

bool cmp(pair<int,double> i, pair<int,double> j)
{
    return (abs(i.second) > abs(j.second));
}


int main(int agrc, char ** argv)
{
    const int nUser = 944;
    const int nMovie = 1683;
    string path = "c:\\data-rs\\80train.txt";
    ifstream ifs(path.c_str(),ifstream::in);

    double **table = new double*[nUser];
    for(int i = 0; i < nMovie; i++)
        table[i] = new double[nMovie];
    for (int i = 0; i < nUser; i++)
        for (int j = 0; j < nMovie; j++)
                    table[i][j] = 0;

    int user, movie, score, time, c =0;
    while(ifs>>user>>movie>>score>>time)
    {
        //c++;
        table[user][movie] = score;
        //cout<<table[user][movie]<<"        "<<c<<endl;
    }
    ifs.close();

    double *ave = new double[nUser];
    for (int i = 0; i < nUser; i++)
        ave[i] = 0;

    for (int i = 1; i < nUser; i++)
    {
        double sum = 0;
        int c = 0;
        for (int j = 1; j < nMovie; j++)
        {
            if (0 != table[i][j])
            {
                c++;
                sum += table[i][j];
            }
        }
        ave[i] = 1.0 * sum / c;
        //cout<<i<<"     "<<sum<<"    "<<c<<"    "<<ave[i]<<endl;
    }


    double **similarity = new double*[nUser];
    for (int i = 0; i < nUser; i++)
            similarity[i] = new double[nUser];
    for (int i = 0; i < nUser; i++)
        for (int j = 0; j < nUser; j++)
            similarity[i][j] = 0;

    for (int i = 1; i < nUser; i++)
    {
        for (int j = i + 1; j < nUser; j++)
        {
            double sum_of_product = 0;
            double sum_of_square1 = 0, sum_of_square2 = 0;
            //int cnt = 0;
            for (int k = 1; k < nMovie; k++)
            {
                if ((0 != table[i][k]) && (0 != table[j][k]))
                {
                    //cout<<table[i][k]<<"         "<<table[j][k]<<endl;
                    sum_of_product += (table[i][k] - ave[i]) * (table[j][k] - ave[j]);
                    sum_of_square1 += (table[i][k] - ave[i]) * (table[i][k] - ave[i]);
                    sum_of_square2 += (table[j][k] - ave[j]) * (table[j][k] - ave[j]);
                    //cnt++;
                    //cout<<sum_of_product<<"            "<<sum_of_square1<<"      "<<sum_of_square2<<endl;
                }
            }
            double pro = sqrt(sum_of_square1) * sqrt(sum_of_square2);
            if (pro == 0)
                similarity[i][j] = similarity[j][i] = 0;
            else
                similarity[i][j] = similarity[j][i] = sum_of_product / pro;
            //cout<<i<<"  "<<j<<"   "<<similarity[i][j]<<endl;
        }
    }

    int knn = 25;

    path = "c:\\data-rs\\test.txt";
    ifs.open(path.c_str(),ifstream::in);

    double mae = -1;
    int cnt = 0;
    while(ifs>>user>>movie>>score>>time)
    {
        pair<int, double> simi_pair[nUser];
        for (int i = 1; i < nUser; i++)
        {
            simi_pair[i] = make_pair(user, similarity[user][i]);
            //cout<<user<<"   "<<i<<"    "<<similarity[user][i]<<endl;
            //cout<<user<<"   "<<i<<"    "<<simi_pair[i].second<<endl;
        }
        sort(simi_pair, simi_pair + nUser, cmp);

        double sum_of_simi = 0;
        double sum_of_abs = 0;
        int i =0;
        for (i = 0; i < knn; i++)
        {
            sum_of_simi += simi_pair[i].second *
                    (table[simi_pair[i].first][movie] - ave[simi_pair[i].first]);
            sum_of_abs += abs(simi_pair[i].second);
            //cout<<user<<"   "<<i<<"    "<<simi_pair[i].second<<endl;
        }

        int result = (int)(ave[user] + sum_of_simi / sum_of_abs + 0.5);

        if (result < 1)
            result = 1;
        if (result > 5)
            result =5;

        mae += abs(result - score);
        cnt++;
        //if (i == knn) break;
    }
    mae /= cnt;
    cout<<mae;
    ifs.close();
}
