#pragma once

#include<QObject>
#include<QString>

#include<QtQml/qqmlregistration.h>
#include"qaddr_bundle.hpp"



using namespace qiota::qblocks;
using namespace qiota;

class BlockSender : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    BlockSender(QObject *parent = nullptr):QObject(parent){};

    Q_INVOKABLE void addOutput(std::shared_ptr<qblocks::Output> out){the_outputs_.push_back(out);}
    Q_INVOKABLE void addBundle(qiota::AddressBundle bundle,quint16 reference=0)
    {
        the_bundles_.push_back(std::pair(bundle,reference));
    }
    Q_INVOKABLE void send(void);


signals:

void notEnoughFunds(quint64);

private:
    std::vector<std::shared_ptr<qblocks::Output>> the_outputs_;
    std::vector<std::pair<qiota::AddressBundle,quint16>> the_bundles_;

};
