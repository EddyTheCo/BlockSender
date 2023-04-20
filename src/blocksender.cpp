#include"blocksender.hpp"

#include"QJsonDocument"
#include"QJsonObject"
#include"nodeConnection.hpp"

using namespace qiota;

void BlockSender::send(void)
{
    if(the_outputs_.size())
    {
        auto  info=Node_Conection::rest_client->get_api_core_v2_info();
        QObject::connect(info,&Node_info::finished,this,[=]( ){
            info->deleteLater();
            quint64 outtotal=0;
            for(const auto& v: the_outputs_)
            {
                outtotal+=Client::get_deposit(v,info);
            }
            c_array Commitments;
            std::vector<std::shared_ptr<Input>> the_inputs_;
            quint64 intotal=0;
            for(const auto& v: the_bundles_)
            {
                the_outputs_.insert(the_outputs_.end(),v.first.ret_outputs.begin(),v.first.ret_outputs.end());
                the_inputs_.insert(the_inputs_.end(),v.first.inputs.begin(),v.first.inputs.end());
                Commitments+=v.first.Inputs_hash;
                intotal+=v.first.amount;
            }
            if(outtotal==intotal)
            {
                auto Inputs_Commitment=Block::get_inputs_Commitment(Commitments);

                auto essence=std::shared_ptr<qblocks::Essence>
                        (new Transaction_Essence(info->network_id_,the_inputs_,Inputs_Commitment,the_outputs_));

                std::vector<std::shared_ptr<Unlock>> the_unlocks;

                for(auto& v: the_bundles_)
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

                auto trpay=std::shared_ptr<qblocks::Payload>(new Transaction_Payload(essence,the_unlocks));
                auto block_=Block(trpay);
                Node_Conection::rest_client->send_block(block_);
            }
            else
            {
                if(outtotal>intotal)
                {
                    emit notEnoughFunds(outtotal-intotal);
                }
            }

        });


    }
}

