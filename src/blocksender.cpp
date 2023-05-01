#include"blocksender.hpp"
#include"nodeConnection.hpp"

using namespace qiota;

void BlockSender::send(void)
{

    if(the_outputs_.size())
    {

        auto outsvar=the_outputs_;
        auto bundvar=the_bundles_;

        the_outputs_.clear();
        the_bundles_.clear();
        auto  info=Node_Conection::rest_client->get_api_core_v2_info();
        QObject::connect(info,&Node_info::finished,this,[=]( ){

            auto bund=bundvar;
            pvector<const Output> c_outputs;
            quint64 outtotal=0;
            for(const auto& v: outsvar)
            {
                if(v->amount_<Client::get_deposit(v,info))
                {
                    v->amount_=Client::get_deposit(v,info);
                }

                outtotal+=v->amount_;
                c_outputs.push_back(std::const_pointer_cast<Output>(v));
            }

            c_array Commitments;
            pvector<const Input> the_inputs_;
            quint64 intotal=0;
            for(const auto& v: bund)
            {
                c_outputs.insert(c_outputs.end(),v.first.ret_outputs.begin(),v.first.ret_outputs.end());
                the_inputs_.insert(the_inputs_.end(),v.first.inputs.begin(),v.first.inputs.end());
                Commitments+=v.first.Inputs_hash;
                intotal+=v.first.amount;
            }
            qDebug()<<"outtotal:"<<outtotal;
            qDebug()<<"intotal:"<<intotal;

            if(outtotal<=intotal&&outtotal>0)
            {
                auto Inputs_Commitment=Block::get_inputs_Commitment(Commitments);


                if(outtotal<intotal)
                {
                    auto addr_bundle=Account::get_addr({0,0,0});
                    const auto eddAddr=addr_bundle.get_address();
                    const auto addUnlcon=Unlock_Condition::Address(eddAddr);
                    auto BaOut=Output::Basic(intotal-outtotal,{addUnlcon});

                    if(intotal-outtotal>=Client::get_deposit(BaOut,info))
                    {
                        c_outputs.push_back(BaOut);
                        auto essence=Essence::Transaction(info->network_id_,the_inputs_,Inputs_Commitment,c_outputs);

                        pvector<const Unlock> the_unlocks;

                        for(auto& v: bund)
                        {
                            if(v.first.get_address()->type()==Address::Ed25519_typ)
                            {
                                v.first.create_unlocks(essence->get_hash());
                            }
                            else
                            {
                                v.first.create_unlocks(essence->get_hash(),v.second);
                            }
                            the_unlocks.insert(the_unlocks.end(),v.first.unlocks.begin(),v.first.unlocks.end());
                        }

                        auto trpay=Payload::Transaction(essence,the_unlocks);

                        auto block_=Block(trpay);

                        QJsonObject outdata;

                        outdata.insert("transactionId",trpay->get_id().toHexString());
                        QJsonArray outids;
                        for(quint16 i=0;i<outsvar.size();i++)
                        {
                            auto v=trpay->get_id();
                            v.append(i);
                            outids.push_back(v.toHexString());

                        }
                        outdata.insert("outIds",outids);
                        emit sent(outdata);
                        Node_Conection::rest_client->send_block(block_);
                    }
                    else
                    {
                        emit notEnoughFunds(Client::get_deposit(BaOut,info)- outtotal+intotal);
                    }

                }


            }
            else
            {

                emit notEnoughFunds(outtotal-intotal);

            }
            info->deleteLater();

        });


    }
}

