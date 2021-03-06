-- translation for StandardPackage

local t = {
	["standard_cards"] = "标准版",

	["slash"] = "杀",
	[":slash"] = "基本牌<br />使用时机：出牌阶段限一次。<br />使用目标：你攻击范围内的一名其他角色。<br />作用效果：你对目标角色造成1点伤害。",
	["slash-jink"] = "%src 使用了【杀】，请使用一张【闪】",
	["@multi-jink-start"] = "%src 使用了【杀】，你须连续使用 %arg 张【闪】",
	["@multi-jink"] = "%src 使用了【杀】，你须再使用 %arg 张【闪】",
	["@slash_extra_targets"] = "请选择此【杀】的额外目标",

	["fire_slash"] = "火杀",
	[":fire_slash"] = "基本牌<br />使用时机：出牌阶段限一次。<br />使用目标：你攻击范围内的一名其他角色。<br />作用效果：你对目标角色造成1点火焰伤害。",

	["thunder_slash"] = "雷杀",
	[":thunder_slash"] = "基本牌<br />使用时机：出牌阶段限一次。<br />使用目标：你攻击范围内的一名其他角色。<br />作用效果：你对目标角色造成1点雷电伤害。",

	["jink"] = "闪",
	[":jink"] = "基本牌<br />使用时机：以你为目标的【杀】生效前。<br />使用目标：以你为目标的【杀】。<br />作用效果：抵消目标【杀】对你产生的效果。",
	["#NoJink"] = "%from 不能使用【<font color=\"yellow\"><b>闪</b></font>】响应此【<font color=\"yellow\"><b>杀</b></font>】",

	["peach"] = "桃",
	[":peach"] = "基本牌<br />使用时机：1、出牌阶段；2、当一名角色处于濒死状态时。<br />使用目标：1、已受伤的包括你在内的一名角色；2、该角色。<br />作用效果：1、目标角色回复1点体力；2、该角色回复1点体力",

	["analeptic"] = "酒",
	[":analeptic"] = "基本牌<br />使用时机：1、出牌阶段，每回合限一次；2、当你处于濒死状态时。<br />使用目标：1、包括你在内的一名角色。2、你。<br />作用效果：1、目标角色使用的下一张【杀】的伤害值基数+1，直到回合结束。2、你回复1点体力。",
	["#UnsetDrankEndOfTurn"] = "%from 的出牌阶段结束，【<font color=\"yellow\"><b>酒</b></font>】的效果消失",

	["Crossbow"] = "诸葛连弩",
	[":Crossbow"] = "装备牌·武器<br />攻击范围：１<br />技能：<font color=\"blue\"><b>锁定技，</b></font>你使用【杀】无次数限制。",

	["DoubleSword"] = "雌雄双股剑",
	[":DoubleSword"] = "装备牌·武器<br />攻击范围：２<br />技能：每当你使用【杀】指定一名异性的目标角色后，你可以令其选择一项：1.弃置一张手牌；2.令你摸一张牌。",
	["double-sword-card"] = "%src 发动了【雌雄双股剑】效果，你须弃置一张手牌，或令 %src 摸一张牌",

	["SixSwords"] = "吴六剑",
	[":SixSwords"] = "攻击范围：2<br/>技能：<font color=\"blue\"><b>锁定技，</b></font>与你势力相同的其他角色的攻击范围+1。",

	["Triblade"] = "三尖两刃刀",
	[":Triblade"] = "攻击范围：3<br/>技能：每当你使用【杀】对目标角色造成伤害后，你可以弃置一张手牌，对该角色距离为1的另一名角色造成1点伤害。",

	["QinggangSword"] = "青釭剑",
	[":QinggangSword"] = "装备牌·武器<br />攻击范围：２<br />技能：<font color=\"blue\"><b>锁定技，</b></font>每当你使用【杀】指定一名目标角色后，你无视其防具。",

	["Spear"] = "丈八蛇矛",
	[":Spear"] = "装备牌·武器<br />攻击范围：３<br />技能：你可以将两张手牌当【杀】使用或打出。",

	["Axe"] = "贯石斧",
	[":Axe"] = "装备牌·武器<br />攻击范围：３<br />技能：每当你使用的【杀】被目标角色使用的【闪】抵消时，你可以弃置两张牌，令此【杀】依然生效。",
	["@Axe"] = "你可以弃置两张牌令此【杀】继续造成伤害",
	["~Axe"] = "选择两张牌→点击确定",

	["KylinBow"] = "麒麟弓",
	[":KylinBow"] = "装备牌·武器<br />攻击范围：５<br />技能：每当你使用【杀】对目标角色造成伤害时，你可以弃置其装备区里的一张坐骑牌。",
	["KylinBow:dhorse"] = "+1坐骑",
	["KylinBow:ohorse"] = "-1坐骑",

	["EightDiagram"] = "八卦阵",
	[":EightDiagram"] = "装备牌·防具<br />技能：每当你需要使用或打出【闪】时，你可以进行判定，若结果为红色，你视为使用或打出了一张【闪】。",

	["standard_ex_cards"] = "标准版EX",

	["RenwangShield"] = "仁王盾",
	[":RenwangShield"] = "装备牌·防具<br />技能：<font color=\"blue\"><b>锁定技，</b></font>黑色【杀】对你无效。",

	["IceSword"] = "寒冰剑",
	[":IceSword"] = "装备牌·武器<br />攻击范围：２<br />技能：每当你使用【杀】对目标角色造成伤害时，若该角色有牌，你可以防止此伤害，依次弃置其两张牌。",

	["Fan"] = "朱雀羽扇",
	[":Fan"] = "装备牌·武器<br />攻击范围：４<br />技能：你可以将一张普通【杀】当火【杀】使用；你可以将视为使用一张【杀】改为视为使用一张火【杀】。",

	["SilverLion"] = "白银狮子",
	[":SilverLion"] = "装备牌·防具<br />技能：<font color=\"blue\"><b>锁定技，</b></font>每当你受到多于1点的伤害时，你防止多余的伤害；<font color=\"blue\"><b>锁定技，</b></font>每当你失去装备区里的【白银狮子】时，你回复1点体力。",
	["#SilverLion"] = "%from 的防具【%arg2】防止了 %arg 点伤害，减至 <font color=\"yellow\"><b>1</b></font> 点",

	["Vine"] = "藤甲",
	[":Vine"] = "装备牌·防具<br />技能：<font color=\"blue\"><b>锁定技，</b></font>【南蛮入侵】、【万箭齐发】和普通【杀】对你无效；<font color=\"blue\"><b>锁定技，</b></font>每当你受到火焰伤害时，你令此伤害+1。",
	["#VineDamage"] = "%from 的防具【<font color=\"yellow\"><b>藤甲</b></font>】效果被触发，火焰伤害由 %arg 点增加至 %arg2 点",

	["Horse"] = "坐骑",
	[":+1 horse"] = "装备牌·坐骑<br />坐骑效果：其他角色与你的距离+1。",
	["JueYing"] = "绝影",
	["DiLu"] = "的卢",
	["ZhuaHuangFeiDian"] = "爪黄飞电",
	[":-1 horse"] = "装备牌·坐骑<br />坐骑效果：你与其他角色的距离-1。",
	["ChiTu"] = "赤兔",
	["DaYuan"] = "大宛",
	["ZiXing"] = "紫骍",

	["amazing_grace"] = "五谷丰登",
	[":amazing_grace"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：所有角色。<br />执行动作：（使用时）你从亮出牌堆顶的X张牌（X为全场角色的数量）。<br />作用效果：每名目标角色获得这些牌中（剩余）的任意一张。",

	["god_salvation"] = "桃园结义",
	[":god_salvation"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：所有角色。<br />作用效果：每名目标角色回复1点体力。",

	["savage_assault"] = "南蛮入侵",
	[":savage_assault"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：所有其他角色。<br />作用效果：每名目标角色需打出一张【杀】，否则受到1点伤害。",
	["savage-assault-slash"] = "%src 使用了【南蛮入侵】，请打出一张【杀】来响应",

	["archery_attack"] = "万箭齐发",
	[":archery_attack"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：所有其他角色。<br />作用效果：每名目标角色需打出一张【闪】，否则受到1点伤害。",
	["archery-attack-jink"] = "%src 使用了【万箭齐发】，请打出一张【闪】以响应",

	["collateral"] = "借刀杀人",
	[":collateral"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：装备区里有武器牌且其攻击范围内有使用【杀】的合法目标的一名其他角色。<br />执行动作：你（在选择该角色为目标的同时）选择目标角色攻击范围内的使用【杀】的一个合法目标A。<br />作用效果：目标角色需对A使用一张【杀】，否则将装备区里的武器牌交给你。",
	["collateral-slash"] = "%dest 使用了【借刀杀人】，请对 %src 使用一张【杀】",
	["#CollateralSlash"] = "%from 选择了此【<font color=\"yellow\"><b>杀</b></font>】的目标 %to",

	["duel"] = "决斗",
	[":duel"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：一名其他角色。<br />作用效果：由目标角色开始，其与你轮流打出一张【杀】，直到其中一方未打出【杀】为止。未打出【杀】的一方受到另一方造成的1点伤害。",
	["duel-slash"] = "%src 对你【决斗】，你需要打出一张【杀】",

	["ex_nihilo"] = "无中生有",
	[":ex_nihilo"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：包括你在内的一名角色。<br />作用效果：目标角色摸两张牌。",

	["snatch"] = "顺手牵羊",
	[":snatch"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：距离为1且区域里有牌的一名角色。<br />作用效果：你获得目标角色区域里的一张牌。",

	["dismantlement"] = "过河拆桥",
	[":dismantlement"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：区域里有牌的一名其他角色。<br />作用效果：你弃置目标角色区域里的一张牌。",

	["nullification"] = "无懈可击",
	[":nullification"] = "锦囊牌<br />使用时机：一张锦囊牌对一个目标生效前。<br />使用目标：此牌。<br />作用效果：抵消此牌对该目标产生的效果。",

	["heg_nullification"] = "无懈可击·国",
	[":heg_nullification"] = "锦囊牌<br />使用时机：1、一张锦囊牌对一个目标生效前；2、一张锦囊牌对一名目标角色生效前。<br />使用目标：此牌。<br />作用效果：1、抵消此牌对该目标产生的效果；2、抵消此牌对该目标角色以及与其势力相同的所有尚未结算的目标角色产生的效果。",

	["indulgence"] = "乐不思蜀",
	[":indulgence"] = "延时锦囊牌<br />使用时机：出牌阶段。<br />使用目标：一名其他角色。<br />作用效果：目标角色进行判定，若结果不为<font color=\"red\">♥</font>，目标角色跳过出牌阶段。",

	["lightning"] = "闪电",
	[":lightning"] = "延时锦囊牌<br />使用时机：出牌阶段。<br />使用目标：你。<br />目标角色进行判定，若结果为黑桃2~9，则目标角色受到3点无来源的雷电伤害，然后将此【闪电】置入弃牌堆。",

	["iron_chain"] = "铁索连环",
	[":iron_chain"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：一至两名角色。<br />每名目标角色选择一项：1.横置武将牌；2. 重置武将牌。<br />◆你可以重铸此牌。",

	["fire_attack"] = "火攻",
	[":fire_attack"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：有手牌的一名角色。<br />作用效果：目标角色展示一张手牌，然后你可以弃置与此牌花色相同的一张手牌，若如此做，该角色受到1点火焰伤害。",
	["fire-attack-card"] = "您可以弃置一张与 %dest 所展示卡牌相同花色(%arg)的牌对 %dest 造成1点火焰伤害",
	["@fire-attack"] = "%src 展示的牌的花色为 %arg，请弃置一张与其相同花色的手牌",

	["supply_shortage"] = "兵粮寸断",
	[":supply_shortage"] = "延时锦囊牌<br />使用时机：出牌阶段。<br />使用目标：距离为1的一名角色。<br />作用效果：目标角色进行判定，若结果不为♣，目标角色跳过摸牌阶段。",
	
	["await_exhausted"] = "以逸待劳",
	[":await_exhausted"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：你和与你势力相同的所有角色。<br />作用效果：每名目标角色摸两张牌，然后每名目标角色弃置两张牌。",

	["known_both"] = "知己知彼",
	[":known_both"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：一名其他角色。<br />作用效果：你观看目标角色的一张暗置的武将牌或其手牌。<br />◆你可以重铸此牌。",

	["befriend_attacking"] = "以逸待劳",
	[":befriend_attacking"] = "锦囊牌<br />使用时机：出牌阶段。<br />使用目标：有明置武将牌且与你势力不同的一名角色。<br />作用效果：目标角色摸一张牌，然后你摸三张牌。",

}

local ohorses = { "ChiTu", "DaYuan", "ZiXing" }
local dhorses = { "ZhuaHuangFeiDian", "DiLu", "JueYing", "HuaLiu" }

for _, horse in ipairs(ohorses) do
	t[":" .. horse] = t[":-1 horse"]
end

for _, horse in ipairs(dhorses) do
	t[":" .. horse] = t[":+1 horse"]
end

return t