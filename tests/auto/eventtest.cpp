/*
    Copyright (C) 2010  Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "qgsttest.h"
#include <QGst/Event>
#include <QGst/Object>
#include <QGst/Message>

class EventTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void baseTest();
    void copyTest();
    void flushStartTest();
    void flushStopTest();
    void eosTest();
    void newSegmentTest();
    void sinkMessageTest();
    void qosTest();
    void seekTest();
    void navigationTest();
    void latencyTest();
    void stepTest();
};

void EventTest::baseTest()
{
    QGst::Structure s("mystructure");
    QGst::EventPtr evt = QGst::Event::create(QGst::EventCustomDownstream, s);

    QVERIFY(evt->type()==QGst::EventCustomDownstream);
    QCOMPARE(evt->typeName(), QString("custom-downstream"));

    QGst::SharedStructure ss = evt->internalStructure();
    QVERIFY(ss.isValid());

    ss.setValue("myfield", 365);
    QCOMPARE(ss.value("myfield").get<int>(), 365);

    QVERIFY(evt->timestamp());
    QVERIFY(evt->source()==NULL);

    evt->setSequenceNumber(123445);
    QCOMPARE(evt->sequenceNumber(), static_cast<quint32>(123445));
}

void EventTest::copyTest()
{
    QGst::Structure s("mystructure");
    s.setValue("myfield", 365);
    QGst::EventPtr evt = QGst::Event::create(QGst::EventCustomUpstream, s);

    QGst::EventPtr evt2 = evt->copy();

    QCOMPARE(evt->type(), evt2->type());
    QCOMPARE(evt->timestamp(), evt2->timestamp());

    QGst::SharedStructure ss = evt2->internalStructure();
    QVERIFY(ss.isValid());
    QCOMPARE(ss.value("myfield").get<int>(), 365);
}

void EventTest::flushStartTest()
{
    QGst::EventPtr evt = QGst::FlushStartEvent::create();
    QVERIFY(evt->type()==QGst::EventFlushStart);
    QCOMPARE(evt->typeName(), QString("flush-start"));
};

void EventTest::flushStopTest()
{
    QGst::EventPtr evt = QGst::FlushStopEvent::create();
    QVERIFY(evt->type()==QGst::EventFlushStop);
    QCOMPARE(evt->typeName(), QString("flush-stop"));
};

void EventTest::eosTest()
{
    QGst::EventPtr evt = QGst::EosEvent::create();
    QVERIFY(evt->type()==QGst::EventEos);
    QCOMPARE(evt->typeName(), QString("eos"));
};

void EventTest::newSegmentTest()
{
    QGst::NewSegmentEventPtr evt = QGst::NewSegmentEvent::create(true, 1.0, 0.5, QGst::FormatTime, 12345,
                                                       234567, 12346);
    QVERIFY(evt->type()==QGst::EventNewSegment);
    QCOMPARE(evt->typeName(), QString("newsegment"));

    QVERIFY(evt->isUpdate());
    QCOMPARE(evt->rate(), 1.0);
    QCOMPARE(evt->appliedRate(), 0.5);
    QVERIFY(evt->format() == QGst::FormatTime);
    QCOMPARE(evt->start(), static_cast<qint64>(12345));
    QCOMPARE(evt->stop(), static_cast<qint64>(234567));
    QCOMPARE(evt->position(), static_cast<qint64>(12346));
};

//TODO GST_EVENT_TAG

void EventTest::sinkMessageTest()
{
    QGst::MessagePtr msg = QGst::BufferingMessage::create(QGst::ObjectPtr(), 90);
    QGst::SinkMessageEventPtr evt = QGst::SinkMessageEvent::create(msg);
    QVERIFY(evt->type()==QGst::EventSinkMessage);
    QCOMPARE(evt->typeName(), QString("sink-message"));

    QGst::MessagePtr msg2 = evt->message();
    QVERIFY(msg2->type() == msg->type());
    QGst::BufferingMessagePtr msg3 = msg2.dynamicCast<QGst::BufferingMessage>();
    QCOMPARE(msg3->percent(), 90);
};

void EventTest::qosTest()
{
    QGst::QosEventPtr evt = QGst::QosEvent::create(123.4, 23455, 98765432);
    QVERIFY(evt->type()==QGst::EventQos);
    QCOMPARE(evt->typeName(), QString("qos"));

    QCOMPARE(evt->proportion(), 123.4);
    QCOMPARE(evt->diff(), static_cast<qint64>(23455));
    QCOMPARE(evt->timestamp(), static_cast<quint64>(98765432));
};

void EventTest::seekTest()
{
    QGst::SeekEventPtr evt = QGst::SeekEvent::create(1.0, QGst::FormatTime, QGst::SeekFlagFlush,
                                                     QGst::SeekTypeSet, 1000000,
                                                     QGst::SeekTypeNone, 500000000);
    QVERIFY(evt->type()==QGst::EventSeek);
    QCOMPARE(evt->typeName(), QString("seek"));

    QVERIFY(evt->format()==QGst::FormatTime);
    QVERIFY(evt->flags()==QGst::SeekFlagFlush);
    QVERIFY(evt->startType()==QGst::SeekTypeSet);
    QCOMPARE(evt->start(), static_cast<qint64>(1000000));
    QVERIFY(evt->stopType()==QGst::SeekTypeNone);
    QCOMPARE(evt->stop(), static_cast<qint64>(500000000));
};

void EventTest::navigationTest()
{
    QGst::Structure s("mystructure");
    s.setValue("myfield", 365);
    QGst::NavigationEventPtr evt = QGst::NavigationEvent::create(s);

    QVERIFY(evt->type()==QGst::EventNavigation);
    QCOMPARE(evt->typeName(), QString("navigation"));

    QGst::SharedStructure ss = evt->internalStructure();
    QVERIFY(ss.isValid());
    QCOMPARE(ss.value("myfield").get<int>(), 365);
}

void EventTest::latencyTest()
{
    QGst::LatencyEventPtr evt = QGst::LatencyEvent::create(43210);
    QVERIFY(evt->type()==QGst::EventLatency);
    QCOMPARE(evt->typeName(), QString("latency"));

    QCOMPARE(evt->latency(), static_cast<quint64>(43210));
};

void EventTest::stepTest()
{
    QGst::StepEventPtr evt = QGst::StepEvent::create(QGst::FormatTime, 100000, 0.5, true, false);
    QVERIFY(evt->type()==QGst::EventStep);
    QCOMPARE(evt->typeName(), QString("step"));

    QVERIFY(evt->format()==QGst::FormatTime);
    QCOMPARE(evt->amount(), static_cast<quint64>(100000));
    QCOMPARE(evt->rate(), 0.5);
    QVERIFY(evt->flush());
    QVERIFY(!evt->intermediate());
};

QTEST_APPLESS_MAIN(EventTest)

#include "moc_qgsttest.cpp"
#include "eventtest.moc"


