#ifndef ANIMAL_H
#define ANIMAL_H

#include <QMainWindow>
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui
{
class Animal;
}
QT_END_NAMESPACE

class Animal : public QMainWindow
{
    Q_OBJECT

public:
    Animal(QWidget *parent = nullptr);
    ~Animal();

private slots:
    void on_btn_reset_clicked();

    void on_btn_forward_clicked();

    void on_btn_reverse_clicked();

private:
    Ui::Animal *ui;

    void clear();  // 复原所有规则和特征

    struct Rule
    {
        int no;  // 规则序号
        QVector<int> in;  // 规则前提
        int out;  // 规则结论
        bool used = false;  // 规则被用过
    };
    struct Fact
    {
        int no;  // 事实（特征）序号
        QString name;  // 对应的名称
        bool flag = false;  // 是否被选中
    };

    QVector<Fact> fact;
    QVector<Rule> rule;

    QCheckBox* cb[100];  // GUI选项统一到指针数组中

    int num_checkbox, num_ans, num_rule;  // 读取特征的个数、可推出结果的个数、规则的个数

    bool find(int t);  // 递归查找某个对象是否符合

};
#endif // ANIMAL_H
