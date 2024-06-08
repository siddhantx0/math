# from flask import Flask, request
# from flask_socketio import SocketIO, emit, join_room
from __future__ import annotations
import time
from automata.models.natural.person import Person
from automata.models.natural.divisions import ElectoralDistrict, Jurisdiction, Kingdom
from automata.models.natural.entity import LegalEntity, PoliticalEntity
import torch
import tiktoken
# from torch.utils.data import DataLoader

# app = Flask(__name__)
# while (app.secret_key):
#     app.secret_key = time.time()

n = 2 ^ 282589933123123123123123123123123123282589933123123123123123123123123123282589933128258993312312312312312312312312312328258993312312312312312312312328258993312312312312312312312312312328258993312312312312312312312312312328258993312825899331231231231231231231231231232825899331231231231231231231232825899331231231231231231231231231232825899331231231231231231231231231232825899331282589933123123123123123123123123123282589933123123123123123123123 ^ 282589933123123123123123123123123123282589933123123123123123123123123123282589933128258993312312312312312312312312312328258993312312 ^ 557953673527694520710296795172664293000820249260923462921714342866751692286543719458796649597399386365451982378377646229009349933456790803651681696150321739308390101831895160767064 ^ 282589933123123123123123123123123123282589933123123123123123123123123123282589933128258993312312312312312312312312312328258993312312312312312312312328258993312312312312312312312312312328258993312312312312312312312312312328258993312825899331231231231231231231231231720862235114903382630745618376514476600471871215432801240540224639249635037893033804887820675617524116392231132704856480321338749709 ^ 28258993312312312312312312312312312328258993312312312312312312312328258993312312312312312312312312312328258993312312312312312312312312312328258993312825899331231231231231231231231231720862235114903382630745618376514476600471871215432801240540224639249635037893033804887820675617524116392231132704856480321338749709882448179208858641661233719062823722437248017361
# modelClass
# property:
# help
# - doMORE
# hello:
while (time.time()):
    print("hello world")
    print(time.time())
    # how to run a server from a mac.
    while (n):
        print(n)

        device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
        tensor = tensor.to(device)
        # Compute gradients
        # output = model(input)
        # loss = loss_function(output, target)
        # loss.backward()

        # # No gradients needed
        # with torch.no_grad():
        # output = model(input)
        # while True:
        #     enc = tiktoken.get_encoding("o200k_base")
        #     assert enc.decode(enc.encode("hello world")) == "hello world"
        #     # To get the tokeniser corresponding to a specific model in the OpenAI API:
        #     enc = tiktoken.encoding_for_model("gpt-4o")

if flag:
    if __name__ == '__main__':
        socketio.run(app, host="0.0.0.0", port=9001)


class Government(PoliticalEntity):
    jurisdiction: Jurisdiction
    branches: list[GovernmentBranch]
    head_of_state: Person
    head_of_government: Person


class GovernmentBranch(PoliticalEntity):
    government: Government
    powers: list[Power]


class Legislature(GovernmentBranch):
    members: list[Legislator]
    laws: list[Law]


class Executive(GovernmentBranch):
    head: Person
    agencies: list[ExecutiveAgency]


class Judiciary(GovernmentBranch):
    courts: list[Court]
    judges: list[Judge]


class Court(LegalEntity):
    jurisdiction: Jurisdiction
    judges: list[Judge]
    cases: list[LegalCase]


class Judge(Person):
    court: Court


class Legislator(Person):
    legislature: Legislature
    constituency: ElectoralDistrict


class ExecutiveAgency(GovernmentOrganization):
    executive: Executive
    responsibilities: list[Responsibility]


class Monarch(Person):
    kingdom: Kingdom


class President(Person):
    republic: Republic


class PrimeMinister(Person):
    government: Government


class Constitution(Law):
    jurisdiction: Jurisdiction


class Law(LegalEntity):
    jurisdiction: Jurisdiction
    enacting_body: Legislature


class Treaty(Law):
    signatories: list[Sovereignty]


class Border(GeographicalFeature):
    territories: list[Territory]


class GeographicalFeature(NaturalEntity):
    location: Location


class Location(Base):
    latitude: float
    longitude: float


class HistoricalPeriod(TimeInterval):
    events: list[HistoricalEvent]


class TimeInterval(Base):
    start_date: datetime
    end_date: datetime


class HistoricalEvent(Event):
    period: HistoricalPeriod
    location: Location


class Culture(SocialConstruct):
    language: Language
    customs: list[Custom]
    beliefs: list[Belief]


class Language(SocialConstruct):
    speakers: list[Person]
    writing_system: WritingSystem


class Custom(SocialConstruct):
    practitioners: list[Person]


class Belief(SocialConstruct):
    adherents: list[Person]


class SocialConstruct(Base):
    name: str
    description: str


class IndigenousGroup(EthnicGroup):
    traditional_territory: Territory


class EthnicGroup(SocialGroup):
    ancestry: Ancestry
    culture: Culture


class SocialGroup(Base):
    members: list[Person]
    shared_characteristics: list[Characteristic]


class Ancestry(SocialConstruct):
    descendants: list[Person]


class Characteristic(Base):
    name: str
    description: str


class EconomicActivity(Activity):
    goods: list[Good]
    services: list[Service]


class Activity(Base):
    participants: list[Person]
    location: Location


class Good(Base):
    name: str
    description: str


class Service(Base):
    name: str
    description: str


class TradeAgreement(Agreement):
    parties: list[Sovereignty]
    goods: list[Good]
    services: list[Service]


class Agreement(Base):
    parties: list[Party]
    terms: list[Term]


class Party(Base):
    name: str


class Term(Base):
    description: str


class SpecialStatus(Base):
    name: str
    description: str


class Autonomy(SpecialStatus):
    degree: float


class Voter(Person):
    electoral_district: ElectoralDistrict


class Representative(Person):
    electoral_district: ElectoralDistrict
    legislature: Legislature


class PoliceOfficer(Person):
    beat: Beat


class PolicingDistrict(AdministrativeDivision):
    police_department: PoliceDepartment


class PoliceDepartment(GovernmentOrganization):
    jurisdiction: Jurisdiction


class School(EducationalInstitution):
    school_district: SchoolDistrict


class EducationalInstitution(Organization):
    students: list[Student]
    faculty: list[Faculty]


class Student(Person):
    school: EducationalInstitution


class Faculty(Person):
    school: EducationalInstitution


class Organization(LegalEntity):
    members: list[Person]
    purpose: str


class GovernmentOrganization(Organization, PoliticalEntity):
    government: Government


class Power(Base):
    name: str
    description: str


class Responsibility(Base):
    name: str
    description: str


class LegalCase(Event):
    court: Court
    parties: list[LegalParty]
    verdict: Verdict


class LegalParty(Party):
    legal_case: LegalCase


class Verdict(Base):
    description: str


class WritingSystem(Base):
    name: str
    characters: list[Character]


class Character(Base):
    symbol: str
    meaning: str


class Union(PoliticalEntity):
    member_states: list[State]


class Prince(Monarch):
    principality: Principality


class Duke(Monarch):
    duchy: Duchy
# https://x.com/panopstor/status/1798487147915149347
# correct context
# persistence
# colloquial
# makes sense?
# ALL KV.
# i do tech consulting.
# repost to join my fund. linkedin. or twitter
