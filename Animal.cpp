#include "Animal.h"
#include "ui_Animal.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QQueue>

Animal::Animal(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Animal)
{
    ui->setupUi(this);

    cb[0] = ui->cb_milk;   // 将GUI界面的QCheckBox对应至指针数组cb中
    cb[1] = ui->cb_hair;
    cb[2] = ui->cb_feather;
    cb[3] = ui->cb_fly;
    cb[4] = ui->cb_oviparous;
    cb[5] = ui->cb_claws;
    cb[6] = ui->cb_teeth;
    cb[7] = ui->cb_look;
    cb[8] = ui->cb_meat;
    cb[9] = ui->cb_hoof;
    cb[10] = ui->cb_ruminate;
    cb[11] = ui->cb_yellow;
    cb[12] = ui->cb_blackstrips;
    cb[13] = ui->cb_blackspots;
    cb[14] = ui->cb_longleg;
    cb[15] = ui->cb_longneck;
    cb[16] = ui->cb_darkspots;
    cb[17] = ui->cb_white;
    cb[18] = ui->cb_ifly;
    cb[19] = ui->cb_blackwhite;
    cb[20] = ui->cb_swim;
    cb[21] = ui->cb_enfly;
    cb[22] = ui->cb_wind;
    cb[23] = ui->cb_mammal;
    cb[24] = ui->cb_bird;
    cb[25] = ui->cb_carnivores;
    cb[26] = ui->cb_ungulate;
    cb[27] = ui->cb_cloven;
    cb[28] = ui->cb_jump;
    cb[29] = ui->cb_lip;
    cb[30] = ui->cb_mouse;
    cb[31] = ui->cb_feet;

    QFile file("rule1.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "警告", "文件打开失败");
        QApplication::exec();
    }
    QTextStream stream(&file); //用文本流读取文件
    stream.setCodec("UTF-8");

    stream >> num_checkbox >> num_ans >> num_rule; // 读取特征的个数、可推出结果的个数、规则的个数

    fact = QVector<Fact>(num_checkbox + num_ans);
    for (int i = 0; i < num_checkbox + num_ans; i++)
    {
        int temp;
        stream >> temp;
        fact[temp].no = temp;       // 特征序号应与文件中保持一致
        stream >> fact[temp].name;  // 读取特征名字
    }

    stream.readLine();
    rule = QVector<Rule>(num_rule);
    for (int i = 0; i < num_rule; i++)
    {
        QString temp = stream.readLine();
        QStringList t = temp.split(" ");
        for (int j = 0; j < t.length() - 1; j++)
            rule[i].in.push_back(t[j].toInt());   // 每行的前面数字为前提
        rule[i].out = t[t.length() - 1].toInt();  // 最后一个数字为结论
        rule[i].no = i;
    }
}

Animal::~Animal()
{
    delete ui;
}

void Animal::clear()  // 复原所有规则和特征
{
    ui->text->clear();
    for (auto& t : rule)
        t.used = false;
    for (auto& t : fact)
        t.flag = false;
}

void Animal::on_btn_reset_clicked()  // 清空所有选项
{
    for (int i = 0; i < num_checkbox; i++)
    {
        cb[i]->setCheckState(Qt::Unchecked);
    }
    clear();
}

void Animal::on_btn_forward_clicked()  // 正向搜索
{
    clear();

    for (int i = 0; i < num_checkbox; i++)
    {
        if (cb[i]->checkState() == Qt::Checked)
            fact[i].flag = true;   // 每次查询，记录选中的事实
    }

    QQueue<Rule*> q;  // 待用规则集
    bool first = true;
    while (!q.empty() | first)
    {
        if (!first)
        {
            Rule* t = q.front();  // 从待用规则集中选出一条
            q.pop_front();
            for (int t : t->in)
                fact[t].flag = false;  // 将前提取消选中
            fact[t->out].flag = true;  // 将结论选中
            QString temp = QString("使用规则%1:").arg(t->no);
            for (int t : t->in)
                temp += fact[t].name + " ";
            temp += "--> " + fact[t->out].name + "\n";
            ui->text->append(temp);

            if (t->out >= num_checkbox && t->out < num_ans + num_checkbox)  // 如果结论是某个满足的结果，则推理成功
            {
                ui->text->append("识别结果为：" + fact[t->out].name + "\n");
                return;
            }
        }
        first = false;
        for (int i = 0; i < num_rule; i++)  // 遍历规则，查找符合所有前提的规则
        {
            if (rule[i].used)  // 已用过的规则不再考虑
                continue;
            int j;
            for (j = 0; j < rule[i].in.size(); j++)
                if (!fact[rule[i].in[j]].flag)
                    break;
            if (j == rule[i].in.size())  // 前提全部匹配
            {
                rule[i].used = true;
                q.push_back(&rule[i]);  // 规则加入待用规则集
            }
        }
    }

    ui->text->append("未找到结果\n");
}

/* 递归查找某个对象是否符合
 * para:待查找的特征序号
 * retn:是否符合事实
 */

bool Animal::find(int t)
{
    if (fact[t].flag == true)  // 已被选中的事实直接返回true
        return true;
    for (int j = 0; j < num_rule; j++)  // 否则遍历规则
    {
        if (rule[j].out == t)  // 规则的结论为待查找的特征
        {
            QString temp = QString("使用规则%1:").arg(j);
            for (int v : rule[j].in)
                temp += fact[v].name + " ";
            temp += "--> " + fact[rule[j].out].name + "\n";
            ui->text->append(temp);
            //            rule[j].used = true;
            bool ok = true;
            for (auto v : rule[j].in)  // 查找规则的前提
                if (find(v) == false)  // 如果一个或多个前提中有一个未找到，则必不符合
                {
                    ok = false;
                    QString temp = QString("规则%1:").arg(j);
                    temp += fact[v].name + " 无法找到\n";
                    ui->text->append(temp);
                    break;
                }
            if (ok == true)  // 如果规则的前提全部符合，则此特征符合事实
                return true;
        }
    }
    return false;  // 遍历规则无法找到符合的事实
}
void Animal::on_btn_reverse_clicked()
{
    clear();

    for (int i = 0; i < num_checkbox; i++)
    {
        if (cb[i]->checkState() == Qt::Checked)
            fact[i].flag = true;
    }

    for (int i = num_checkbox; i < num_checkbox + num_ans; i++)
    {
        ui->text->append("尝试动物" + fact[i].name + "\n");
        if (find(i))
        {
            ui->text->append("找到动物为：" + fact[i].name + "\n");
            return;
        }
        for (auto& t : rule)  // 每次改变搜索对象，需要将已用规则清空
            t.used = false;
    }

    ui->text->append("未找到动物\n");
}
