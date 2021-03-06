#include "formation.h"
#include "general.h"
#include "standard.h"
#include "standard-equips.h"
#include "skill.h"
#include "engine.h"
#include "client.h"
#include "serverplayer.h"
#include "room.h"
#include "ai.h"
#include "settings.h"

class Tuntian: public TriggerSkill {
public:
    Tuntian(): TriggerSkill("tuntian") {
        events << CardsMoveOneTime << FinishJudge;
        frequency = Frequent;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (!TriggerSkill::triggerable(player) || player->getPhase() != Player::NotActive) return false;
        if (triggerEvent == CardsMoveOneTime) {
            CardsMoveOneTimeStruct move = data.value<CardsMoveOneTimeStruct>();
            if (move.from == player && (move.from_places.contains(Player::PlaceHand) || move.from_places.contains(Player::PlaceEquip))
                && !(move.to == player && (move.to_place == Player::PlaceHand || move.to_place == Player::PlaceEquip)))
                return true;
        } else if (triggerEvent == FinishJudge) {
            JudgeStar judge = data.value<JudgeStar>();
            if (judge->reason == "tuntian" && judge->isGood())
                player->addToPile("field", judge->card->getEffectiveId());
        }
        return false;
    }

    virtual bool cost(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        return player->askForSkillInvoke("tuntian", data);
    }

    virtual bool effect(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        room->broadcastSkillInvoke("tuntian");
        JudgeStruct judge;
        judge.pattern = ".|heart";
        judge.good = false;
        judge.reason = "tuntian";
        judge.who = player;
        room->judge(judge);
       
        return false;
    }
};

class TuntianDistance: public DistanceSkill {
public:
    TuntianDistance(): DistanceSkill("#tuntian-dist") {
    }

    virtual int getCorrect(const Player *from, const Player *) const{
        if (from->hasShownSkill(Sanguosha->getSkill("tuntian")))
            return -from->getPile("field").length();
        else
            return 0;
    }
};

JixiCard::JixiCard() {
    target_fixed = true;
}

void JixiCard::onUse(Room *room, const CardUseStruct &card_use) const{
    ServerPlayer *dengai = card_use.from;

    QList<int> fields;
    QList<int> total = dengai->getPile("field");
    foreach (int id, total) {
        Snatch *snatch = new Snatch(Card::SuitToBeDecided, -1);
        snatch->addSubcard(id);
        if (!snatch->isAvailable(dengai))
            continue;
        foreach (ServerPlayer *p, room->getAlivePlayers()) {
            if (!snatch->targetFilter(QList<const Player *>(), p, dengai))
                continue;
            if (dengai->isProhibited(p, snatch))
                continue;
            fields << id;
            break;
        }
        delete snatch;
        snatch = NULL;
    }

    if (fields.isEmpty())
        return;

    QList<int> disabled;
    foreach (int id, total) {
        if (!fields.contains(id))
            disabled << id;
    }

    int card_id;
    if (fields.length() == 1)
        card_id = fields.first();
    else {
        room->fillAG(total, dengai, disabled);
        card_id = room->askForAG(dengai, fields, false, "jixi");
        room->clearAG(dengai);

        if (card_id == -1)
            return;
    }

    Snatch *snatch = new Snatch(Card::SuitToBeDecided, -1);
    snatch->setSkillName("jixi");
    snatch->addSubcard(card_id);

    QList<ServerPlayer *> targets;
    foreach (ServerPlayer *p, room->getAlivePlayers()) {
        if (!snatch->targetFilter(QList<const Player *>(), p, dengai))
            continue;
        if (dengai->isProhibited(p, snatch))
            continue;

        targets << p;
    }
    if (targets.isEmpty())
        return;

    room->setPlayerProperty(dengai, "jixi_snatch", snatch->toString());

    CardUseStruct use;
    use.card = snatch;
    use.from = dengai;

    if (room->askForUseCard(dengai, "@@jixi!", "@jixi-target")) {
        foreach (ServerPlayer *p, room->getAlivePlayers()) {
            if (p->hasFlag("JixiSnatchTarget")) {
                room->setPlayerFlag(p, "-JixiSnatchTarget");
                use.to << p;
            }
        }
    } else {
        use.to << targets.at(qrand() % targets.length());
    }
    room->setPlayerProperty(dengai, "jixi_snatch", QString());
    room->useCard(use);
}

JixiSnatchCard::JixiSnatchCard() {
}

bool JixiSnatchCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    const Card *card = Card::Parse(Self->property("jixi_snatch").toString());
    if (card == NULL)
        return false;
    else {
        const Snatch *snatch = qobject_cast<const Snatch *>(card);
        return !Self->isProhibited(to_select, snatch, targets) && snatch->targetFilter(targets, to_select, Self);
    }
}

void JixiSnatchCard::onUse(Room *room, const CardUseStruct &card_use) const{
    foreach (ServerPlayer *to, card_use.to)
        room->setPlayerFlag(to, "JixiSnatchTarget");
}

class Jixi: public ZeroCardViewAsSkill {
public:
    Jixi(): ZeroCardViewAsSkill("jixi") {
        relate_to_place = "head";
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        if (player->getPile("field").isEmpty())
            return false;
        foreach (int id, player->getPile("field")) {
            Snatch *snatch = new Snatch(Card::SuitToBeDecided, -1);
            snatch->setSkillName("jixi");
            snatch->addSubcard(id);
            snatch->deleteLater();
            if (!snatch->isAvailable(player))
                continue;
            foreach (const Player *p, player->getAliveSiblings()) {
                if (!snatch->targetFilter(QList<const Player *>(), p, player))
                    continue;
                if (player->isProhibited(p, snatch))
                    continue;
                return true;
            }
        }
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *, const QString &pattern) const{
        return pattern == "@@jixi!";
    }

    virtual const Card *viewAs() const{
        QString pattern = Sanguosha->currentRoomState()->getCurrentCardUsePattern();
        if (pattern == "@@jixi!") {
            Card *card = new JixiSnatchCard;
            card->setShowSkill(objectName());
            return card;
        } else
            return new JixiCard;
    }
};

class Ziliang: public TriggerSkill {
public:
    Ziliang(): TriggerSkill("ziliang") {
        events << Damaged;
        relate_to_place = "deputy";
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (player == NULL) return false;
        ServerPlayer *dengai = room->findPlayerBySkillName(objectName());
        if (!player->isAlive()) return false;
        if (!dengai || dengai->getPile("field").isEmpty()) return false;
        if ((dengai->willBeFriendWith(player) && player->hasShownOneGeneral()) || dengai->isFriendWith(player)) {
            ask_who = dengai;
            return true;
        }
        return false;
    }

    virtual bool cost(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *dengai = room->findPlayerBySkillName(objectName());
        return room->askForSkillInvoke(dengai, objectName(), data);
    }

    virtual bool effect(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *dengai = room->findPlayerBySkillName(objectName());
        
        int id = room->askForAG(dengai, dengai->getPile("field"), false, objectName());
        if (player == dengai) {
            LogMessage log;
            log.type = "$MoveCard";
            log.from = player;
            log.to << player;
            log.card_str = QString::number(id);
            room->sendLog(log);
        }
        room->obtainCard(player, id);

        return false;
    }
}; 

HuyuanCard::HuyuanCard() {
    will_throw = false;
    handling_method = Card::MethodNone;
}

bool HuyuanCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if (!targets.isEmpty())
        return false;

    const Card *card = Sanguosha->getCard(subcards.first());
    const EquipCard *equip = qobject_cast<const EquipCard *>(card->getRealCard());
    int equip_index = static_cast<int>(equip->location());
    return to_select->getEquip(equip_index) == NULL;
}

void HuyuanCard::onEffect(const CardEffectStruct &effect) const{
    ServerPlayer *caohong = effect.from;
    Room *room = caohong->getRoom();
    room->moveCardTo(this, caohong, effect.to, Player::PlaceEquip,
                     CardMoveReason(CardMoveReason::S_REASON_PUT, caohong->objectName(), "huyuan", QString()));

    const Card *card = Sanguosha->getCard(subcards.first());

    LogMessage log;
    log.type = "$ZhijianEquip";
    log.from = effect.to;
    log.card_str = QString::number(card->getEffectiveId());
    room->sendLog(log);

    QList<ServerPlayer *> targets;
    foreach (ServerPlayer *p, room->getAllPlayers()) {
        if (effect.to->distanceTo(p) == 1 && caohong->canDiscard(p, "he"))
            targets << p;
    }
    if (!targets.isEmpty()) {
        ServerPlayer *to_dismantle = room->askForPlayerChosen(caohong, targets, "huyuan", "@huyuan-discard:" + effect.to->objectName());
        int card_id = room->askForCardChosen(caohong, to_dismantle, "he", "huyuan", false, Card::MethodDiscard);
        room->throwCard(Sanguosha->getCard(card_id), to_dismantle, caohong);
    }
}

class HuyuanViewAsSkill: public OneCardViewAsSkill {
public:
    HuyuanViewAsSkill(): OneCardViewAsSkill("huyuan") {
        response_pattern = "@@huyuan";
        filter_pattern = "EquipCard";
    }

    virtual const Card *viewAs(const Card *originalcard) const{
        HuyuanCard *first = new HuyuanCard;
        first->addSubcard(originalcard->getId());
        first->setSkillName(objectName());
        first->setShowSkill(objectName());
        return first;
    }
};

class Huyuan: public PhaseChangeSkill {
public:
    Huyuan(): PhaseChangeSkill("huyuan") {
        view_as_skill = new HuyuanViewAsSkill;
    }

    virtual bool triggerable(TriggerEvent , Room *room, ServerPlayer *target, QVariant &data, ServerPlayer* &ask_who) const {
        if (!PhaseChangeSkill::triggerable(target)) return false;
        return (target->getPhase() == Player::Finish && !target->isNude());
    }

    virtual bool cost(TriggerEvent , Room *room, ServerPlayer *target, QVariant &data) const {
        return room->askForUseCard(target, "@@huyuan", "@huyuan-equip", -1, Card::MethodNone);
    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        return false;
    }
}; 

HeyiSummon::HeyiSummon() 
    : ArraySummonCard("heyi")
{

}

class Heyi: public BattleArraySkill {
public:
    Heyi(): BattleArraySkill("heyi", BattleArrayType::Formation) {
        events << GeneralShown << GeneralHidden << Death;
        frequency = Compulsory;
    }
    
    virtual bool canPreshow() const{
        return false;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (player == NULL) return false;
        if (triggerEvent == Death) {
            DeathStruct death = data.value<DeathStruct>();
            if (death.who->hasSkill(objectName())) {
                foreach (ServerPlayer *p, room->getAllPlayers())
                    if (p->hasSkill("feiying"))
                        room->detachSkillFromPlayer(p, "feiying", true);
                return false;
            }
        }
        foreach (ServerPlayer *p, room->getAllPlayers())
            if (p->hasSkill("feiying"))
                room->detachSkillFromPlayer(p, "feiying", true);

        if (room->alivePlayerCount() < 4) return false;
        ServerPlayer *caohong = room->findPlayerBySkillName(objectName());
        if (!caohong) return false;
        QList<ServerPlayer *> teammates;
        bool next = true;
        ServerPlayer *next_p = caohong;
        while (next) {
            next_p = qobject_cast<ServerPlayer *>(next_p->getNextAlive());
            if (next_p->isFriendWith(caohong) && !teammates.contains(next_p))
                teammates << next_p;
            else
                next = false;
        }
        next = true;
        next_p = caohong;
        while (next) {
            foreach (ServerPlayer *p, room->getAllPlayers())
                if (p->getNextAlive() == next_p) {
                    next_p = p;
                    break;
                }
            if (next_p->isFriendWith(caohong) && !teammates.contains(next_p))
                teammates << next_p;
            else
                next = false;
        }
        if (teammates.isEmpty()) return false;
        foreach(ServerPlayer *p, teammates)
            room->attachSkillToPlayer(p, "feiying");

        return false;
    }
};

class Feiying: public DistanceSkill {
public:
    Feiying(): DistanceSkill("feiying") {
    }

    virtual int getCorrect(const Player *, const Player *to) const{
        if (to->hasSkill(objectName()))
            return 1;
        else
            return 0;
    }
};

TiaoxinCard::TiaoxinCard() {
}

bool TiaoxinCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select->inMyAttackRange(Self) && to_select != Self;
}

void TiaoxinCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    bool use_slash = false;
    if (effect.to->canSlash(effect.from, NULL, false))
        use_slash = room->askForUseSlashTo(effect.to, effect.from, "@tiaoxin-slash:" + effect.from->objectName());
    if (!use_slash && effect.from->canDiscard(effect.to, "he"))
        room->throwCard(room->askForCardChosen(effect.from, effect.to, "he", "tiaoxin", false, Card::MethodDiscard), effect.to, effect.from);
}

class Tiaoxin: public ZeroCardViewAsSkill {
public:
    Tiaoxin(): ZeroCardViewAsSkill("tiaoxin") {
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("TiaoxinCard");
    }

    virtual const Card *viewAs() const{
        Card *card = new TiaoxinCard;
        card->setShowSkill(objectName());
        return card;
    }

    virtual int getEffectIndex(const ServerPlayer *player, const Card *) const{
        int index = qrand() % 2 + 1;
        if (!player->hasInnateSkill(objectName()) && player->hasSkill("baobian"))
            index += 2;
        return index;
    }
};

class YiZhi: public TriggerSkill {
public:
    YiZhi(): TriggerSkill("yizhi") {
        relate_to_place = "deputy";
        frequency = Compulsory;
        events << GameStart << EventPhaseStart;
    }

    virtual bool canPreshow() const {
        return true;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer * &ask_who) const{
        if (!player || !player->isAlive() || !player->ownSkill(objectName())) return false;
        if (triggerEvent == GameStart) return true;
        else if (triggerEvent == EventPhaseStart && player->getPhase() == Player::Start)
            return (!player->ownSkill("guanxing"));
        return false;
    }

    virtual bool cost(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        if (triggerEvent == EventPhaseStart) {
            if (player->hasSkill(objectName()) && player->askForSkillInvoke("guanxing")) {
                LogMessage log;
                log.from = player;
                log.arg = "guanxing";
                room->sendLog(log);
                return true;
            }
        } else {
            const Skill *guanxing = Sanguosha->getSkill("guanxing");
            if (guanxing != NULL && guanxing->inherits("TriggerSkill")){
                const TriggerSkill *guanxing_trigger = qobject_cast<const TriggerSkill *>(guanxing);
                room->getThread()->addTriggerSkill(guanxing_trigger);
            }
        }

        return false;   //skill is written in Guanxing actrually
    }

    virtual bool effect(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        QList<int> guanxing = room->getNCards(qMin(5, player->aliveCount()));

        LogMessage log;
        log.type = "$ViewDrawPile";
        log.from = player;
        log.card_str = IntList2StringList(guanxing).join("+");
        room->doNotify(player, QSanProtocol::S_COMMAND_LOG_SKILL, log.toJsonValue());

        room->askForGuanxing(player, guanxing, false);

        return false;
    }
};

TianfuSummon::TianfuSummon()
    : ArraySummonCard("tianfu")
{

}

class Tianfu: public BattleArraySkill {
public:
    Tianfu(): BattleArraySkill("tianfu", BattleArrayType::Formation) {
        events << EventPhaseStart << Death;
        relate_to_place = "head";
    }

    virtual bool canPreshow() const{
        return false;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (player == NULL) return false;
        
        ServerPlayer *jiangwei = room->findPlayerBySkillName(objectName());
        if (!jiangwei) return false;
        if (jiangwei->hasSkill("kanpo") && !jiangwei->ownSkill("kanpo"))
            room->detachSkillFromPlayer(jiangwei, "kanpo", true);

        if (room->alivePlayerCount() < 4 || !jiangwei->hasShownSkill(this)) return false;
        QList<ServerPlayer *> teammates;
        bool next = true;
        ServerPlayer *next_p = jiangwei;
        while (next) {
            next_p = qobject_cast<ServerPlayer *>(next_p->getNextAlive());
            if (next_p->isFriendWith(jiangwei) && !teammates.contains(next_p))
                teammates << next_p;
            else
                next = false;
        }
        next = true;
        next_p = jiangwei;
        while (next) {
            foreach (ServerPlayer *p, room->getAllPlayers())
                if (p->getNextAlive() == next_p) {
                    next_p = p;
                    break;
                }
            if (next_p->isFriendWith(jiangwei) && !teammates.contains(next_p))
                teammates << next_p;
            else
                next = false;
        }
        teammates << jiangwei;
        if (teammates.isEmpty()) return false;
        foreach(ServerPlayer *p, teammates)
            if (p->getPhase() != Player::NotActive) {
                room->attachSkillToPlayer(jiangwei, "kanpo");
                break;
            }

        return false;
    }
};

class Shengxi: public TriggerSkill {
public:
    Shengxi(): TriggerSkill("shengxi") {
        events << DamageDone << EventPhaseEnd;
        frequency = Frequent;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (!TriggerSkill::triggerable(player)) return false;
        if (triggerEvent == EventPhaseEnd) {
            if (TriggerSkill::triggerable(player) && player->getPhase() == Player::Play) {
                if (!player->hasFlag("ShengxiDamageInPlayPhase"))
                    return true;
            }
            if (player->hasFlag("ShengxiDamageInPlayPhase"))
                player->setFlags("-ShengxiDamageInPlayPhase");
        } else if (triggerEvent == DamageDone) {
            DamageStruct damage = data.value<DamageStruct>();
            if (damage.from && damage.from->getPhase() == Player::Play && !damage.from->hasFlag("ShengxiDamageInPlayPhase"))
                damage.from->setFlags("ShengxiDamageInPlayPhase");
        }
        return false;
    }

    virtual bool cost(TriggerEvent triggerEvent, Room *, ServerPlayer *player, QVariant &data) const{
        return player->askForSkillInvoke(objectName());
    }

    virtual bool effect(TriggerEvent triggerEvent, Room *, ServerPlayer *player, QVariant &data) const{
        player->drawCards(2);
        
        return false;
    }
};

class Shoucheng: public TriggerSkill {
public:
    Shoucheng(): TriggerSkill("shoucheng") {
        events << CardsMoveOneTime;
        frequency = Frequent;
    }
    
    virtual bool triggerable(TriggerEvent triggerEvent, Room *, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (!TriggerSkill::triggerable(player)) return false;
        CardsMoveOneTimeStruct move = data.value<CardsMoveOneTimeStruct>();
        if (move.from && move.from->isAlive() && move.from->getPhase() == Player::NotActive && move.from->isFriendWith(player)
                && move.from_places.contains(Player::PlaceHand) && move.is_last_handcard)
            return true;

        return false;
    }

    virtual bool cost(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        return player->askForSkillInvoke(objectName());
    }

    virtual bool effect(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        CardsMoveOneTimeStruct move = data.value<CardsMoveOneTimeStruct>();
        room->broadcastSkillInvoke(objectName());
        ServerPlayer *from = (ServerPlayer *)move.from;
        from->drawCards(1);
        return false;
    }
};

#include "jsonutils.h"

ShangyiCard::ShangyiCard() {
}

bool ShangyiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && (!to_select->isKongcheng() || !to_select->hasShownAllGenerals());
}

void ShangyiCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    room->showAllCards(effect.from, effect.to);
    QStringList choices;
    if (!effect.to->isKongcheng())
        choices << "handcards";
    if (!effect.to->hasShownAllGenerals())
        choices << "hidden_general";
    
    QString choice = room->askForChoice(effect.from, "shangyi", 
                                        choices.join("+"), QVariant::fromValue(effect.to));

    if (choice == "handcards") {
        room->showAllCards(effect.to, effect.from);
        QList<int> blacks, reds;
        foreach (int card_id, effect.to->handCards())
            if (Sanguosha->getCard(card_id)->isBlack())
                blacks << card_id;
            else if (Sanguosha->getCard(card_id)->isRed())
                reds << card_id;
        room->fillAG(effect.to->handCards(), effect.from, reds);

        int to_discard = -1;
        to_discard = room->askForAG(effect.from, blacks, true, "shangyi");
        if (to_discard == -1) return;
        room->throwCard(to_discard, effect.to, effect.from);
    } else {
        QStringList list = room->getTag(effect.to->objectName()).toStringList();
        foreach (QString name, list) {
            LogMessage log;
            log.type = "$KnownBothViewGeneral";
            log.from = effect.from;
            log.to << effect.to;
            log.arg = name;
            room->doNotify(effect.from, QSanProtocol::S_COMMAND_LOG_SKILL, log.toJsonValue());
        }
        Json::Value arg(Json::arrayValue);
        arg[0] = QSanProtocol::Utils::toJsonString("shangyi");
        arg[1] = QSanProtocol::Utils::toJsonArray(list);
        room->doNotify(effect.from, QSanProtocol::S_COMMAND_VIEW_GENERALS, arg);
    }
}

class Shangyi: public ZeroCardViewAsSkill {
public:
    Shangyi(): ZeroCardViewAsSkill("shangyi") {
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("ShangyiCard") && !player->isKongcheng();
    }

    virtual const Card *viewAs() const{
        Card *card = new ShangyiCard;
        card->setShowSkill(objectName());
        return card;
    }
};

NiaoxiangSummon::NiaoxiangSummon()
    : ArraySummonCard("niaoxiang")
{

}

class Niaoxiang: public BattleArraySkill {
public:
    Niaoxiang(): BattleArraySkill("niaoxiang", BattleArrayType::Siege) {
        events << TargetConfirmed;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (!TriggerSkill::triggerable(player)) return false;
        if (!player->hasShownSkill(this) || player->aliveCount() < 4) return false;
        CardUseStruct use = data.value<CardUseStruct>();
        if (use.card->isKindOf("Slash")) {
            if (!player->getNextAlive()->isFriendWith(player) && player->getNextAlive(2)->isFriendWith(player)) {
                if ((use.from == player || use.from == qobject_cast<ServerPlayer *>(player->getNextAlive(2)))
                    && use.to.contains(qobject_cast<ServerPlayer *>(player->getNext()))) {
                    QVariantList jink_list = use.from->tag["Jink_" + use.card->toString()].toList();
                    for (int i = 0; i < use.to.length(); i++) {
                        if (use.to.at(i) == player->getNext())
                            if (jink_list.at(i).toInt() == 1)
                                jink_list.replace(i, QVariant(2));
                    }
                    use.from->tag["Jink_" + use.card->toString()] = QVariant::fromValue(jink_list);
                }
            }
            for (int i = 0; i < use.to.length(); i++) {
                ServerPlayer *victim = use.to.at(i), *teammate;
                if (victim->getNextAlive() == player) {
                    foreach (ServerPlayer *p, room->getOtherPlayers(victim))
                        if (p->getNextAlive() == victim){
                            teammate = p;
                            break;
                        }
                } else continue;

                if (teammate && teammate->isFriendWith(player) && !victim->isFriendWith(player))
                    if ((use.from == player || use.from == teammate)) {
                        QVariantList jink_list = use.from->tag["Jink_" + use.card->toString()].toList();
                        if (jink_list.at(i).toInt() == 1)
                            jink_list.replace(i, QVariant(2));
                        use.from->tag["Jink_" + use.card->toString()] = QVariant::fromValue(jink_list);
                    }
            }
        }

        return false;
    }
};

class Yicheng: public TriggerSkill {
public:
    Yicheng(): TriggerSkill("yicheng") {
        events << TargetConfirmed;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (!TriggerSkill::triggerable(player)) return false;
        CardUseStruct use = data.value<CardUseStruct>();
        if (!use.card->isKindOf("Slash")) return false;
        foreach (ServerPlayer *p, use.to) {
            if ((p->hasShownOneGeneral() && player->willBeFriendWith(p)) || player->isFriendWith(p)) {
                ask_who = player;
                player->tag["yicheng_target"] = QVariant::fromValue(p);
                return true;
            }
            if (!player->isAlive()) break;
        }

        return false;
    }

    virtual bool cost(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        PlayerStar p = player->tag["yicheng_target"].value<PlayerStar>();
        if (room->askForSkillInvoke(player, objectName(), QVariant::fromValue(p)))
            return true;
        else
            player->tag.remove("yicheng_target");
        return false;
    }

    virtual bool effect(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        PlayerStar p = player->tag["yicheng_target"].value<PlayerStar>();
        player->tag.remove("yicheng_target");
        if (p) {
            p->drawCards(1);
            if (p->isAlive() && p->canDiscard(p, "he"))
                room->askForDiscard(p, objectName(), 1, 1, false, true);
        }
        return false;
    }
}; 

class Qianhuan: public TriggerSkill {
public:
    Qianhuan(): TriggerSkill("qianhuan") {
        events << Damaged << TargetConfirming;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (player == NULL) return false;
        if (triggerEvent == Damaged && player->isAlive()) {
            ServerPlayer *yuji = room->findPlayerBySkillName(objectName());
            if (yuji && ((player->hasShownOneGeneral() && yuji->willBeFriendWith(player)) || player->isFriendWith(yuji))) {
                ask_who = yuji;
                return true;
            }
        } else if (triggerEvent == TargetConfirming) {
            CardUseStruct use = data.value<CardUseStruct>();
            if (!use.card || use.card->getTypeId() == Card::TypeEquip || use.card->getTypeId() == Card::TypeSkill)
                return false;
            if (use.to.length() != 1) return false;
            ServerPlayer *yuji = room->findPlayerBySkillName(objectName());
            if (!yuji || yuji->getPile("sorcery").isEmpty()) return false;
            if ((use.to.first()->hasShownOneGeneral() && yuji->willBeFriendWith(use.to.first())) || use.to.first()->isFriendWith(yuji)) {
                ask_who = yuji;
                return true;
            }
        }
        return false;
    }

    virtual bool cost(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *yuji = room->findPlayerBySkillName(objectName());
        return yuji && room->askForSkillInvoke(yuji, objectName());
    }

    virtual bool effect(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *yuji = room->findPlayerBySkillName(objectName());
        if (!yuji) return false;
        if (triggerEvent == Damaged) {
            room->broadcastSkillInvoke(objectName());

            int id = room->drawCard();
            Card::Suit suit = Sanguosha->getCard(id)->getSuit();
            bool duplicate = false;
            foreach (int card_id, yuji->getPile("sorcery")) {
                if (Sanguosha->getCard(card_id)->getSuit() == suit) {
                    duplicate = true;
                    break;
                }
            }
            yuji->addToPile("sorcery", id);
            if (duplicate) {
                CardMoveReason reason(CardMoveReason::S_REASON_REMOVE_FROM_PILE, QString(), objectName(), QString());
                room->throwCard(Sanguosha->getCard(id), reason, NULL);
            }
        } else if (triggerEvent == TargetConfirming) {
            CardUseStruct use = data.value<CardUseStruct>();
            room->broadcastSkillInvoke(objectName());
            room->notifySkillInvoked(yuji, objectName());
            QList<int> ids = yuji->getPile("sorcery");
            int id = -1;
            if (ids.length() > 1) {
                room->fillAG(ids, yuji);
                id = room->askForAG(yuji, ids, false, objectName());
                room->clearAG(yuji);
            } else {
                id = ids.first();
            }
            CardMoveReason reason(CardMoveReason::S_REASON_REMOVE_FROM_PILE, QString(), objectName(), QString());
            room->throwCard(Sanguosha->getCard(id), reason, NULL);

            LogMessage log;
            if (use.from) {
                log.type = "$CancelTarget";
                log.from = use.from;
            } else {
                log.type = "$CancelTargetNoUser";
            }
            log.to = use.to;
            log.arg = use.card->objectName();
            room->sendLog(log);


            use.to.clear();
            data = QVariant::fromValue(use);
        }

        return false;
    }
};


class Zhendu: public TriggerSkill {
public:
    Zhendu(): TriggerSkill("zhendu") {
        events << EventPhaseStart;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        if (player == NULL) return false;
        if (player->getPhase() != Player::Play) return false;
        ServerPlayer *hetaihou = room->findPlayerBySkillName(objectName());
        if (!hetaihou || !hetaihou->isAlive() || !hetaihou->canDiscard(hetaihou, "h") || hetaihou->getPhase() == Player::Play) return false;
        ask_who = hetaihou;
        return true;
    }

    virtual bool cost(TriggerEvent, Room *room, ServerPlayer *player, QVariant &) const{
        ServerPlayer *hetaihou = room->findPlayerBySkillName(objectName());
        return hetaihou && room->askForCard(hetaihou, ".", "@zhendu-discard", QVariant(), objectName());
    }

    virtual bool effect(TriggerEvent, Room *room, ServerPlayer *player, QVariant &) const{
        ServerPlayer *hetaihou = room->findPlayerBySkillName(objectName());
            
        if (!hetaihou) return false;

        Analeptic *analeptic = new Analeptic(Card::NoSuit, 0);
        analeptic->setSkillName("_zhendu");
        room->useCard(CardUseStruct(analeptic, player, QList<ServerPlayer *>(), true));
        if (player->isAlive())
            room->damage(DamageStruct(objectName(), hetaihou, player));

        return false;
    }
};

class Qiluan: public TriggerSkill {
public:
    Qiluan(): TriggerSkill("qiluan") {
        events << Death << EventPhaseStart;
        frequency = Frequent;
    }

    virtual bool triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const {
        if (player == NULL) return false;
        if (triggerEvent == Death) {
            DeathStruct death = data.value<DeathStruct>();
            if (death.who != player)
                return false;

            ServerPlayer *current = room->getCurrent();
            if (current && (current->isAlive() || death.who == current) && current->getPhase() != Player::NotActive){
                foreach(ServerPlayer *p, room->getAllPlayers())
                    if (TriggerSkill::triggerable(p))
                        room->setPlayerMark(p, objectName(), 1);
            }

            return false;
        } else {
            if (player->getPhase() == Player::NotActive) {
                foreach (ServerPlayer *p, room->getAllPlayers()) {
                    if (p->getMark(objectName()) > 0 && TriggerSkill::triggerable(p)) {
                        room->setPlayerMark(p, objectName(), 0);
                        if (p->isAlive()) {
                            ask_who = p;
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    virtual bool cost(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *hetaihou = room->findPlayerBySkillName(objectName());
        return hetaihou && hetaihou->askForSkillInvoke(objectName());
    }

    virtual bool effect(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *hetaihou = room->findPlayerBySkillName(objectName());
        if (hetaihou)
            hetaihou->drawCards(3);

        return false;
    }
};

FormationPackage::FormationPackage()
    : Package("formation")
{
    General *dengai = new General(this, "dengai", "wei"); // WEI 015
    dengai->addSkill(new Tuntian);
    dengai->addSkill(new TuntianDistance);
    dengai->addSkill(new Jixi);
    dengai->setHeadMaxHpAdjustedValue(-1);
    dengai->addSkill(new Ziliang); 
    related_skills.insertMulti("tuntian", "#tuntian-dist");

    General *caohong = new General(this, "caohong", "wei"); // WEI 018
    caohong->addSkill(new Huyuan);
    caohong->addSkill(new Heyi);

    General *jiangwei = new General(this, "jiangwei", "shu"); // SHU 012 G
    jiangwei->addSkill(new Tiaoxin);
    jiangwei->addSkill(new YiZhi);
    jiangwei->setDeputyMaxHpAdjustedValue(-1);
    jiangwei->addSkill(new Tianfu);

    General *jiangwanfeiyi = new General(this, "jiangwanfeiyi", "shu", 3); // SHU 018 
    jiangwanfeiyi->addSkill(new Shengxi);
    jiangwanfeiyi->addSkill(new Shoucheng);

    General *jiangqin = new General(this, "jiangqin", "wu"); // WU 017
    jiangqin->addSkill(new Shangyi);
    jiangqin->addSkill(new Niaoxiang);

    General *xusheng = new General(this, "xusheng", "wu"); // WU 020
    xusheng->addSkill(new Yicheng);

    General *yuji = new General(this, "yuji", "qun", 3); // QUN 011 G
    yuji->addSkill(new Qianhuan);

    General *hetaihou = new General(this, "hetaihou", "qun", 3, false); // QUN 020
    hetaihou->addSkill(new Zhendu);
    hetaihou->addSkill(new Qiluan); 
    
    addMetaObject<JixiCard>();
    addMetaObject<JixiSnatchCard>();
    addMetaObject<HuyuanCard>();
    addMetaObject<TiaoxinCard>();
    addMetaObject<ShangyiCard>();
    addMetaObject<HeyiSummon>();
    addMetaObject<TianfuSummon>();
    addMetaObject<NiaoxiangSummon>();

    skills << new Feiying;
}

ADD_PACKAGE(Formation)