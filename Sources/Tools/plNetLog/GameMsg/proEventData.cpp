/******************************************************************************
 * This file is part of plNetLog.                                             *
 *                                                                            *
 * plNetLog is free software: you can redistribute it and/or modify           *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * plNetLog is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with plNetLog.  If not, see <http://www.gnu.org/licenses/>.          *
 ******************************************************************************/

#include "proEventData.h"
#include "Factory.h"

enum EventType
{
    kEvtInvalid, kEvtCollision, kEvtPicked, kEvtControlKey,
    kEvtVariable, kEvtFacing, kEvtContained, kEvtActivate,
    kEvtCallback, kEvtResponderState, kEvtMultiStage, kEvtSpawned,
    kEvtClickDrag, kEvtCoop, kEvtOfferLinkBook, kEvtBook,
    kEvtClimbingBlockerHit, kEvtNone
};

void EventData_Factory(QTreeWidgetItem* parent, ChunkBuffer& buffer)
{
    static const char* s_eventTypeNames[] = {
        "Invalid", "Collision", "Picked", "ControlKey", "Variable",
        "Facing", "Contained", "Activate", "Callback", "ResponderState",
        "MultiStage", "Spawned", "ClickDrag", "Coop", "OfferLinkBook",
        "Book", "ClimbingBlockerHit", "None"
    };

    unsigned type = buffer.read<unsigned>();
    if (type < (sizeof(s_eventTypeNames) / sizeof(s_eventTypeNames[0]))) {
        new QTreeWidgetItem(parent, QStringList()
            << QString("Type: %1").arg(s_eventTypeNames[type]));
    } else {
        QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList()
            << QString("Type: %1 (Unknown)").arg(type));
        item->setForeground(0, Qt::red);
    }

    switch (type) {
    case kEvtCollision:
        new QTreeWidgetItem(parent, QStringList()
            << QString("Enter: %1").arg(buffer.read<bool>() ? "True" : "False"));
        Key(parent, "Hitter", buffer);
        Key(parent, "Hittee", buffer);
        break;
    case kEvtPicked:
        Key(parent, "Picker", buffer);
        Key(parent, "Picked", buffer);
        new QTreeWidgetItem(parent, QStringList()
            << QString("Enabled: %1").arg(buffer.read<bool>() ? "True" : "False"));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Hit Point: (%1, %2, %3)").arg(buffer.read<float>())
               .arg(buffer.read<float>()).arg(buffer.read<float>()));
        break;
    case kEvtControlKey:
        new QTreeWidgetItem(parent, QStringList()
            << QString("Key: %1").arg(buffer.read<int>()));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Down: %1").arg(buffer.read<bool>() ? "True" : "False"));
        break;
    case kEvtVariable:
        new QTreeWidgetItem(parent, QStringList()
            << QString("Name: %1").arg(buffer.readSafeString()));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Type: %1").arg(buffer.read<unsigned>()));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Number: %1").arg(buffer.read<float>()));
        Key(parent, "Key", buffer);
        break;
    case kEvtFacing:
        Key(parent, "Facer", buffer);
        Key(parent, "Facee", buffer);
        new QTreeWidgetItem(parent, QStringList()
            << QString("Dot: %1").arg(buffer.read<float>()));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Enabled: %1").arg(buffer.read<bool>() ? "True" : "False"));
        break;
    case kEvtContained:
        Key(parent, "Container", buffer);
        Key(parent, "Containee", buffer);
        new QTreeWidgetItem(parent, QStringList()
            << QString("Entering: %1").arg(buffer.read<bool>() ? "True" : "False"));
        break;
    case kEvtActivate:
        new QTreeWidgetItem(parent, QStringList()
            << QString("Active: %1").arg(buffer.read<bool>() ? "True" : "False"));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Activate: %1").arg(buffer.read<bool>() ? "True" : "False"));
        break;
    case kEvtCallback:
        new QTreeWidgetItem(parent, QStringList()
            << QString("Type: %1").arg(buffer.read<int>()));
        break;
    case kEvtResponderState:
        new QTreeWidgetItem(parent, QStringList()
            << QString("State: %1").arg(buffer.read<int>()));
        break;
    case kEvtMultiStage:
        new QTreeWidgetItem(parent, QStringList()
            << QString("Stage: %1").arg(buffer.read<unsigned>()));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Event: %1").arg(buffer.read<unsigned>()));
        Key(parent, "Avatar", buffer);
        break;
    case kEvtSpawned:
        Key(parent, "Spawner", buffer);
        Key(parent, "Spawnee", buffer);
        break;
    case kEvtCoop:
        new QTreeWidgetItem(parent, QStringList()
            << QString("ID: %1").arg(buffer.read<unsigned>()));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Serial: %1").arg(buffer.read<unsigned short>()));
        break;
    case kEvtOfferLinkBook:
        Key(parent, "Offerer", buffer);
        new QTreeWidgetItem(parent, QStringList()
            << QString("Target Age: %1").arg(buffer.read<unsigned>()));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Offeree: %1").arg(buffer.read<unsigned>()));
        break;
    case kEvtBook:
        new QTreeWidgetItem(parent, QStringList()
            << QString("Event: %1").arg(buffer.read<unsigned>()));
        new QTreeWidgetItem(parent, QStringList()
            << QString("Link ID: %1").arg(buffer.read<unsigned>()));
        break;
    case kEvtClimbingBlockerHit:
        Key(parent, "Blocker", buffer);
        break;
    case kEvtClickDrag:
    case kEvtNone:
        break;
    default:
        {
            new QTreeWidgetItem(parent, QStringList()
                << QString("Unsupported proEventData type (%1)").arg(type));

            QTreeWidgetItem* item = parent;
            while (item->parent())
                item = item->parent();
            QFont warnFont = item->font(0);
            warnFont.setBold(true);
            item->setFont(0, warnFont);
            item->setForeground(0, Qt::red);

            OutputDebugStringA(QString("Unsupported proEventData type (%1)\n")
                               .arg(type).toUtf8().data());
        }
    }
}
