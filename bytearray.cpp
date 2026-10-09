/*
HelperClasses.dll, a collection of useful classes and routines.
Copyright (C) 2023 - 2026    Reinhard Hölscher

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "bytearray.h"
#include <QString>
#include "QFile"

/*! \file bytearray.cpp
 *  \brief In dieser Datei wird die Klasse __ByteArray__ implementiert.
 */

namespace RH {

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __ByteArray__.
         *
         * Die ByteArray-Instanz kann nach ihrer Erstellung bis zu 32 Datenbytes aufnehmen und verwalten.
         * Freizugebender Heapspeicher wird vor der Freigabe nicht gelöscht.
         *
         *      ByteArray *ba = new ByteArray();
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 32 Bytes.
         * Davon sind 0 Bytes mit Daten belegt.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         */
        ByteArray::ByteArray() {
                ba = new Array<char>();
        }

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __ByteArray__ und kopiert ein Zeichenarray hinein.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 28 Bytes.
         * Davon sind 27 Bytes mit Daten belegt (der Bierbestellung).
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         * \param pointer Zeiger auf ein Zeichenarray.
         * \param sec Wenn _true_ wird der Heapspeicher vor seiner Freigabe gelöscht. Der Standardwert der Variablen _sec_ ist _false_.
         */
        ByteArray::ByteArray(const char *pointer, const bool sec) {
                size_t length = strlen(pointer);
                ba = new Array<char>(length + 1, sec);
                for (size_t i = 0; i < length; i++) {
                        (*ba)[i] = pointer[i];
                }
                (*ba)[length] = '\0';
                SetSize(length);
        }

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __ByteArray__.
         *
         * Die ByteArray-Instanz kann nach ihrer Erstellung bis zu __size__ (siehe Variable size) Datenbytes aufnehmen und verwalten.
         *
         *
         *      ByteArray *ba = new ByteArray(4096, true);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 4096 Bytes.
         * Davon sind 0 Bytes mit Daten belegt.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel gelöscht.
         * \param size Die Anzahl der Datenbytes, die diese Instanz nach ihrer Erstellung verwalten kann.
         * \param sec Wenn _true_ wird der Heapspeicher vor seiner Freigabe gelöscht. Der Standardwert der Variablen _sec_ ist _false_.
         */
        ByteArray::ByteArray(const size_t size, const bool sec) {
                ba = new Array<char>(size, sec);
        }

        /*! \brief Konstruktor: Initialisiert eine neue Instanz der Klasse __ByteArray__ und kopiert einen QString hinein.
         *
         *      QString qstr("Ein Glas kaltes Bier bitte!");
         *      ByteArray *ba = new ByteArray(qstr, true);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 27 Bytes.
         * Davon sind alle 27 Bytes mit Daten belegt (der Bierbestellung).
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel gelöscht.
         * \param ref Referenz zu einer QString-Instanz.
         * \param sec Wenn _true_ wird der Heapspeicher vor seiner Freigabe gelöscht. Der Standardwert der Variablen _sec_ ist _false_.
         */
        ByteArray::ByteArray(const QString &ref, const bool sec) {
                QByteArray qba(ref.toUtf8());
                size_t length = qba.size();
                ba = new Array<char>(length, sec);
                ba->SetSize(length);
                for (size_t i = 0; i < length; i++) {
                        (*ba)[i] = qba[static_cast<int>(i)];
                }
        }

        /*! \brief Kopierkonstruktor: Initialisiert eine neue Instanz der Klasse __ByteArray__ und kopiert die Daten von einer bereits existierenden Instanz in diese neue Instanz.
         *
         *      ByteArray *ba1 = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      ByteArray *ba2 = new ByteArray(*ba1);
         *      delete ba1;
         *      delete ba2;
         *
         * In diesem Beispiel enthält die Variable _ba1_ einen Zeiger auf eine ByteArray-Instanz. Diese hat eine Kapazität von 28 Bytes.
         * Davon sind 27 Bytes mit Daten belegt (der Bierbestellung).
         * Die Variable _ba2_ enthält ebenfalls einen Zeiger auf eine (andere) ByteArray-Instanz. Diese hat (ebenfalls) eine Kapazität von 28 Bytes.
         * Auch hier sind davon 27 Bytes mit Daten belegt (der Bierbestellung).
         * Vor der Zerstörung der beiden ByteArray-Instanzen, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         * \param ref Enthält die Referenz zu einer bereits existierenden Instanz aus der kopiert werden soll.
         */
        ByteArray::ByteArray(const ByteArray &ref) {
                ba = new Array<char>(*(ref.ba));
        }

        /*! \brief Destruktor: Zerstört die existierende Instanz der Klasse __ByteArray__.
         *
         * Wenn der Parameter _sec_ bei der Erstellung dieser Instanz auf _true_ gesetzt war, wird der Heapspeicher vor seiner Freigabe gelöscht.
         */
        ByteArray::~ByteArray() {
                delete ba;
        }

        /*! \brief Diese Methode `Append()` fügt die Datenbytes einer anderen ByteArray-Instanz hinter den schon vorhandenen Datenbytes dieser ByteArray-Instanz hinzu.
         *
         * Die Methode prüft zunächst, ob in dieser ByteArray-Instanz noch genügend Platz vorhanden ist. Falls nicht, wird zunächst die Kapazität dieser ByteArray-Instanz erhöht
         * (und mindestens verdoppelt). Anschließend fügt die Methode die neuen Datenbytes hinter den schon vorhandenen Datenbytes hinzu.
         *
         *      ByteArray *ba1 = new ByteArray("Ein Glas kaltes Bier");
         *      ByteArray *ba2 = new ByteArray(" bitte!");
         *      ba1->Append(*ba2);
         *      delete ba1;
         *      delete ba2;
         *
         * In diesem Beispiel enthält die Variable _ba1_ einen Zeiger auf eine ByteArray-Instanz. Diese hat eine Kapazität von 21 Bytes.
         * Davon sind 20 Bytes mit Daten belegt.
         * Die Variable _ba2_ enthält ebenfalls einen Zeiger auf eine (andere) ByteArray-Instanz. Diese hat eine Kapazität von 8 Bytes.
         * Hier sind davon 7 Bytes mit Daten belegt.
         * Mit der Methode `Append()` werden nun die Bytes aus der zweiten ByteArray-Instanz an die erste ByteArray-Instanz angefügt.
         * Damit das möglich ist, muss zunächst die Kapazität in der ersten ByteArray-Instanz erhöht werden.
         * Sie wird auf 42 Bytes verdoppelt. Davon sind nach dem Append-Vorgang 27 Bytes belegt.
         * Vor der Zerstörung der beiden ByteArray-Instanzen, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         * \param ref Verweis auf die ByteArray-Instanz, aus der gelesen werden soll und deren Datenbytes hinzugefügt werden sollen.
         */
        void ByteArray::Append(const ByteArray &ref) {
                ba->Append(*(ref.ba));
        }

        /*! \brief Diese Methode `Append()` fügt ein Datenbyte hinter den schon vorhandenen Datenbytes hinzu.
         *
         * Die Methode prüft zunächst, ob in der ByteArray-Instanz noch Platz vorhanden ist. Ist dies nicht der Fall, verdoppelt es zunächst die Kapazität der ByteArray-Instanz.
         * Anschließend fügt die Methode das neue Datenbyte hinter den schon vorhandenen Datenbytes hinzu.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte", true);
         *      ba->Append('!');
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 27 Bytes.
         * Davon sind 26 Bytes mit Daten belegt (der Bierbestellung ohne Ausrufezeichen).
         * Mit der Methode `Append()` wird noch ein Ausrufezeichen angefügt. Die Kapazität bleibt gleich und alle 27 Bytes sind nun mit Daten belegt.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel gelöscht.
         * \param ref Verweis auf das Datenbyte, das hinzugefügt werden soll.
         */
        void ByteArray::Append(const char &ref) {
                ba->Append(ref);
        }

        /*! \brief Diese Methode `At()` gibt das Datenbyte zurück, das unter dem nullbasierten Index der ByteArray-Instanz vorhanden ist.
         *
         * Die Methode prüft zunächst, ob der Indexwert innerhalb eines gültigen Wertebereichs liegt (auf ein bereits verwaltetes Datenbyte verweist).
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst. Falls ja, wird das gewünschte Datenbyte zurückgegeben.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      char erg = ba->At(12);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _erg_ den Buchstaben __t__, da an der Indexposition __12__ der Buchstabe __t__ steht.
         *
         * \param index Der Index im Datenbereich, dessen Datenbyte zurückgegeben werden soll.
         * \return Das gewünschte (indexierte) Datenbyte.
         * \sa operator[]()
         */
        char &ByteArray::At(const size_t index) const {
                return ba->At(index);
        }

        /*! \brief Diese Methode `Clear()` setzt die Anzahl der verwalteten Bytes auf 0.
         *
         * Nach dem Aufruf der Methode `Clear()` beträgt die Anzahl der verwalteten Bytes 0.
         * Die Kapazität der ByteArray-Instanz bleibt aber unverändert und auch die Daten sind immer noch vorhanden.
         * Wenn der Parameter _sec_ bei der Erstellung dieser Instanz auf _true_ gesetzt war, wird der Heapspeicher gelöscht aber nicht freigegeben.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      size_t length1 = ba->Size();
         *      ba->Clear();
         *      size_t length2 = ba->Size();
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 28 Bytes.
         * Davon sind 27 Bytes mit Daten belegt (der Bierbestellung). Die Variable _length1_ hat den Wert __27__. Nach der Methode `Clear()`
         * hat die Variable _length2_ den Wert __0__. Die Daten in der ByteArray-Instanz sind aber noch vorhanden.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         */
        void ByteArray::Clear() {
                ba->Clear();
        }

        /*! \brief Diese Methode `Compare()` vergleicht diese ByteArray-Instanz mit einer übergebenen ByteArray-Instanz.
         *
         * Der Vergleich berücksichtigt die Groß-/Kleinschreibung, wenn _caseSensitive_ auf _true_ gesetzt ist (dies ist der Standardwert).
         *
         *      ByteArray *ba1 = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      ByteArray *ba2 = new ByteArray(*ba1);
         *      int result = ba1->Compare(*ba2);
         *      delete ba1;
         *      delete ba2;
         *
         * Die Variable _result_ hat in diesem Beispiel den Wert __0__.
         * \param ref Verweis auf eine ByteArray-Instanz, mit der verglichen werden soll.
         * \param caseSensitive Wenn _true_ wird die Groß-/Kleinschreibung beachtet (ansonsten nicht).
         * \return -1, wenn diese Instanz kleiner ist als die Vergleichsinstanz. 0, wenn beide Instanzen gleich sind und 1, wenn diese Instanz größer ist als die Vergleichsinstanz.
         * \sa IsEqual()
         */

        int ByteArray::Compare(const ByteArray &ref, const bool caseSensitive) {
                int result;
                size_t refSize = ref.Size();
                size_t thisSize = this->Size();
                if (refSize == thisSize) {
                        return Compare(ref, 0, 0, refSize, caseSensitive);
                }
                if (refSize < thisSize) {
                        result = Compare(ref, 0, 0, refSize, caseSensitive);
                        if (result == 0) {
                                result = -1;
                        }
                } else {
                        result = Compare(ref, 0, 0, thisSize, caseSensitive);
                        if (result == 0) {
                                result = 1;
                        }
                }
                return result;
        }

        /*! \brief Diese Methode `Compare()` vergleicht Bytes von diese ByteArray-Instanz mit den Bytes einer übergebenen ByteArray-Instanz.
         *
         * Die Methode prüft zunächst, ob die Quell- und Zielindexwerte innerhalb der zulässigen Wertebereiche liegen.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Danach findet der eigentliche Vergleichsprozess statt.
         * Der Vergleich berücksichtigt die Groß-/Kleinschreibung, wenn _caseSensitive_ auf _true_ gesetzt ist (dies ist der Standardwert).
         *
         *      ByteArray *ba1 = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      ByteArray *ba2 = new ByteArray("Zwei Glas kaltes Bier bitte!");
         *      int result = ba1->Compare(*ba2, 5, 4, 16);
         *      delete ba1;
         *      delete ba2;
         *
         * Die Variable _result_ hat in diesem Beispiel den Wert __0__.
         * In der ersten ByteArray-Instanz (Ein __Glas kaltes Bier__ bitte!) und der zweiten ByteArray-Instanz (Zwei __Glas kaltes Bier__ bitte!)
         * wurden die __fett__ markierten Bereiche miteinander verglichen.
         * \param ref Verweis auf eine ByteArray-Instanz, mit der verglichen werden soll.
         * \param sindex Der nullbasierende Startindex in der übergebenen ByteArray-Instanz.
         * \param tindex Der nullbasierende Startindex in dieser ByteArray-Instanz.
         * \param size Die Anzahl der zu vergleichenden Bytes.
         * \param caseSensitive Wenn _true_ wird die Groß-/Kleinschreibung beachtet (ansonsten nicht).
         * \return -1, wenn diese Instanz kleiner ist als die Vergleichsinstanz. 0, wenn beide Instanzen gleich sind und 1, wenn diese Instanz größer ist als die Vergleichsinstanz.
         * \sa IsEqual()
         */
        int ByteArray::Compare(const ByteArray &ref, const size_t sindex, const size_t tindex, const size_t size, const bool caseSensitive) {
                if ((sindex + size) > ref.ba->Size()) {
                        throw std::out_of_range("Source Index");
                }
                if ((tindex + size) > this->Size()) {
                        throw std::out_of_range("Target Index");
                }
                char cs;
                char ct;
                for (size_t i = 0; i < size; i++) {
                        cs = (*ref.ba)[sindex + i];
                        ct = (*this)[tindex + i];
                        if (!caseSensitive) {
                                cs = tolower(cs);
                                ct = tolower(ct);
                        }
                        if (cs < ct) {
                                return 1;
                        }
                        if (cs > ct) {
                                return -1;
                        }
                }
                return 0;
        }

        /*! \brief Diese Methode `Compare()` vergleicht Bytes von dieser ByteArray-Instanz mit den Bytes aus einem Datenfeld (vom Typ char*).
         *
         * Die Methode prüft zunächst, ob die Quell- und Zielindexwerte innerhalb der zulässigen Wertebereiche liegen.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Danach findet der eigentliche Vergleichsprozess statt.
         * Der Vergleich berücksichtigt die Groß-/Kleinschreibung, wenn _caseSensitive_ auf _true_ gesetzt ist (dies ist der Standardwert).
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      int result = ba->Compare("Zwei Glas kaltes Bier bitte!", 5, 4, 16);
         *      delete ba;
         *
         * Die Variable _result_ hat in diesem Beispiel den Wert __0__.
         * In der ByteArray-Instanz (Ein __Glas kaltes Bier__ bitte!) und in dem Datenfeld (vom Typ char*) (Zwei __Glas kaltes Bier__ bitte!)
         * wurden die __fett__ markierten Bereiche miteinander verglichen.
         * \param pointer Der Zeiger auf das Datenfeld, mit dem verglichen werden soll.
         * \param sindex Der nullbasierende Startindex in der übergebenen ByteArray-Instanz.
         * \param tindex Der nullbasierende Startindex in dieser ByteArray-Instanz.
         * \param size Die Anzahl der zu vergleichenden Bytes.
         * \param caseSensitive Wenn _true_ wird die Groß-/Kleinschreibung beachtet (ansonsten nicht).
         * \return -1, wenn diese Instanz kleiner ist als die Vergleichsinstanz. 0, wenn beide Instanzen gleich sind und 1, wenn diese Instanz größer ist als die Vergleichsinstanz.
         * \sa IsEqual()
         */
        int ByteArray::Compare(const char *pointer, const size_t sindex, const size_t tindex, const size_t size, const bool caseSensitive) {
                if ((tindex + size) > this->Size()) {
                        throw std::out_of_range("Target Index");
                }
                char cs;
                char ct;
                for (size_t i = 0; i < size; i++) {
                        cs = pointer[sindex + i];
                        ct = (*this)[tindex + i];
                        if (!caseSensitive) {
                                cs = tolower(cs);
                                ct = tolower(ct);
                        }
                        if (cs < ct) {
                                return 1;
                        }
                        if (cs > ct) {
                                return -1;
                        }
                }
                return 0;
        }

        /*! \brief Diese Methode `Copy()` kopiert Bytes von einer anderen ByteArray-Instanz in diese ByteArray-Instanz.
         *
         * Die Methode prüft zunächst, ob die Quell- und Zielindexwerte innerhalb der zulässigen Wertebereiche liegen.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Die Methode prüft anschließend, ob die Kapazität im Datenfeld für den Schreibvorgang ausreicht.
         * Wenn nicht, wird die Kapazität in dieser ByteArray-Instanz erhöht.
         * Danach erfolgt der eigentliche Kopiervorgang. Falls erforderlich, wird die Variable _size_ angepasst.
         *
         *      ByteArray *ba1 = new ByteArray("Ein Glas kaltes Bier");
         *      ByteArray *ba2 = new ByteArray("Hefeweizen bitte!");
         *      ba1->Copy(*ba2, 0, 16, 17);
         *      delete ba1;
         *      delete ba2;
         *
         * In diesem Beispiel enthält die Variable _ba1_ einen Zeiger auf eine ByteArray-Instanz. Diese hat eine Kapazität von 21 Bytes.
         * Davon sind 20 Bytes mit Daten belegt.
         * Die Variable _ba2_ enthält ebenfalls einen Zeiger auf eine (andere) ByteArray-Instanz. Diese hat eine Kapazität von 18 Bytes.
         * Hier sind davon 17 Bytes mit Daten belegt.
         * Mit der Methode `Copy()` werden nun die Bytes aus der zweiten ByteArray-Instanz an die erste ByteArray-Instanz kopiert.
         * Damit das möglich ist, muss zunächst die Kapazität in der ersten ByteArray-Instanz erhöht werden.
         * Sie wird auf 42 Bytes verdoppelt. Davon sind nach dem Kopiervorgang 33 Bytes belegt.
         * Der Text in der ersten ByteArray-Instanz lautet nun: "Ein Glas kaltes Hefeweizen bitte!".
         * Vor der Zerstörung der beiden ByteArray-Instanzen, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         * \param ref Verweis auf die Quell-ByteArray-Instanz, aus der kopiert werden soll.
         * \param sindex Der nullbasierte Quellindex in der Quell-ByteArray-Instanz, aus der kopiert werden soll.
         * \param tindex Der nullbasierte Zielindex in dieser ByteArray-Instanz, in die geschrieben werden soll.
         * \param size Die Anzahl der zu kopierenden Bytes.
         */
        void ByteArray::Copy(const ByteArray &ref, const size_t sindex, const size_t tindex, const size_t size) {
                if ((sindex + size) > ref.ba->Size()) {
                        throw std::out_of_range("Source Index");
                }
                if (tindex > ba->Size()) {
                        throw std::out_of_range("Target Index");
                }
                size_t newLength = tindex + size;
                size_t capacity = ba->Capacity();
                if (newLength > capacity) {
                        while (newLength > capacity) {
                                capacity = capacity << 1;
                        }
                        ba->SetCapacity(capacity);
                }
                for (size_t i = 0; i < size; i++) {
                        (*this)[tindex + i] = (*ref.ba)[sindex + i];
                }
                if (ba->Size() < (tindex + size)) {
                        ba->SetSize(tindex + size);
                }
        }

        /*! \brief Diese Methode `Copy()` kopiert Bytes aus einem Datenfeld (vom Typ char*) in diese ByteArray-Instanz.
         *
         * Die Methode prüft zunächst, ob der Zielindexwert innerhalb des zulässigen Wertebereiches liegt.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Die Methode prüft anschließend, ob die Kapazität im Datenfeld für den Schreibvorgang ausreicht.
         * Wenn nicht, wird die Kapazität in dieser ByteArray-Instanz erhöht.
         * Danach erfolgt der eigentliche Kopiervorgang. Falls erforderlich, wird die Variable _size_ angepasst.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier");
         *      ba->Copy("Hefeweizen bitte!", 0, 16, 17);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf eine ByteArray-Instanz. Diese hat eine Kapazität von 21 Bytes.
         * Davon sind 20 Bytes mit Daten belegt.
         * Das Datenfeld (vom Typ char*) hat eine Länge von 17 Bytes und enthält den Text: "Hefeweizen bitte!".
         * Mit der Methode `Copy()` werden nun die Bytes aus dem Datenfeld in die ByteArray-Instanz kopiert.
         * Damit das möglich ist, muss zunächst die Kapazität in der ByteArray-Instanz erhöht werden.
         * Sie wird auf 42 Bytes verdoppelt. Davon sind nach dem Kopiervorgang 33 Bytes belegt.
         * Der Text in der ByteArray-Instanz lautet nun: "Ein Glas kaltes Hefeweizen bitte!".
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         * \param pointer Der Zeiger auf das Datenfeld, aus dem kopiert werden soll.
         * \param sindex Der nullbasierte Quellindex in dem Datenfeld, aus dem kopiert werden soll.
         * \param tindex Der nullbasierte Zielindex in dieser ByteArray-Instanz, in die geschrieben werden soll.
         * \param size Die Anzahl der zu kopierenden Bytes.
         */
        void ByteArray::Copy(const char *pointer, const size_t sindex, const size_t tindex, const size_t size) {
                if (tindex > ba->Size()) {
                        throw std::out_of_range("Target Index");
                }
                size_t newLength = tindex + size;
                size_t capacity = ba->Capacity();
                if (newLength > capacity) {
                        while (newLength > capacity) {
                                capacity = capacity << 1;
                        }
                        ba->SetCapacity(capacity);
                }
                for (size_t i = 0; i < size; i++) {
                        (*this)[tindex + i] = pointer[sindex + i];
                }
                if (ba->Size() < (tindex + size)) {
                        ba->SetSize(tindex + size);
                }
        }

        /*! \brief Diese Methode `Copy()` kopiert Bytes von einer QByteArray-Instanz in diese ByteArray-Instanz.
         *
         * Die Methode prüft zunächst, ob der Zielindexwert innerhalb des zulässigen Wertebereiches liegt.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Die Methode prüft anschließend, ob die Kapazität im Datenfeld für den Schreibvorgang ausreicht.
         * Wenn nicht, wird die Kapazität in dieser ByteArray-Instanz erhöht.
         * Danach erfolgt der eigentliche Kopiervorgang. Falls erforderlich, wird die Variable _size_ angepasst.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier");
         *      QByteArray qba("Hefeweizen bitte!");
         *      ba->Copy(qba, 0, 4, 17);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf eine ByteArray-Instanz. Diese hat eine Kapazität von 21 Bytes.
         * Davon sind 20 Bytes mit Daten belegt.
         * Die QByteArray-Instanz hat eine Länge von 17 Bytes und enthält den Text: "Hefeweizen bitte!".
         * Mit der Methode `Copy()` werden nun die Bytes aus der QByteArray-Instanz in die ByteArray-Instanz kopiert.
         * Der Text in der ByteArray-Instanz lautet nun: "Ein Hefeweizen bitte!". Alle 21 Bytes sind nun belegt.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         * \param ref Verweis auf die QByteArray-Instanz, aus der kopiert werden soll.
         * \param sindex Der nullbasierte Quellindex in der QByteArray-Instanz, aus der kopiert werden soll.
         * \param tindex Der nullbasierte Zielindex in dieser ByteArray-Instanz, in die geschrieben werden soll.
         * \param size Die Anzahl der zu kopierenden Bytes.
         */
        void ByteArray::Copy(const QByteArray &ref, const size_t sindex, const size_t tindex, const size_t size) {
                if ((sindex + size) > static_cast<size_t>(ref.size())) {
                        throw std::out_of_range("Source Index");
                }
                if (tindex > ba->Size()) {
                        throw std::out_of_range("Target Index");
                }
                size_t newLength = tindex + size;
                size_t capacity = ba->Capacity();
                if (newLength > capacity) {
                        while (newLength > capacity) {
                                capacity = capacity << 1;
                        }
                        ba->SetCapacity(capacity);
                }
                for (size_t i = 0; i < size; i++) {
                        (*this)[tindex + i] = ref[static_cast<int>(sindex + i)];
                }
                if (ba->Size() < (tindex + size)) {
                        ba->SetSize(tindex + size);
                }
        }

        /*! \brief Diese Methode `Copy()` kopiert Bytes von einer QString-Instanz in diese ByteArray-Instanz.
         *
         * Die Methode prüft zunächst, ob der Zielindexwert innerhalb des zulässigen Wertebereiches liegt.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Die Methode prüft anschließend, ob die Kapazität im Datenfeld für den Schreibvorgang ausreicht.
         * Wenn nicht, wird die Kapazität in dieser ByteArray-Instanz erhöht.
         * Danach erfolgt der eigentliche Kopiervorgang. Falls erforderlich, wird die Variable _size_ angepasst.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier", true);
         *      QString qstr("Hefeweizen bitte!");
         *      ba->Copy(qstr, 0, 4, 17);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf eine ByteArray-Instanz. Diese hat eine Kapazität von 21 Bytes.
         * Davon sind 20 Bytes mit Daten belegt.
         * Die QString hat eine Länge von 17 Bytes und enthält den Text: "Hefeweizen bitte!".
         * Mit der Methode `Copy()` werden nun die Bytes aus der QString-Instanz in die ByteArray-Instanz kopiert.
         * Der Text in der ByteArray-Instanz lautet nun: "Ein Hefeweizen bitte!". Alle 21 Bytes sind nun belegt.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel gelöscht.
         * \param ref Verweis auf die QString-Instanz, aus der kopiert werden soll.
         * \param sindex Der nullbasierte Quellindex in der QString-Instanz, aus der kopiert werden soll.
         * \param tindex Der nullbasierte Zielindex in dieser ByteArray-Instanz, in die geschrieben werden soll.
         * \param size Die Anzahl der zu kopierenden Bytes.
         */

        void ByteArray::Copy(const QString &ref, const size_t sindex, const size_t tindex, const size_t size) {
                QByteArray qba(ref.toUtf8());
                if ((sindex + size) > static_cast<size_t>(qba.size())) {
                        throw std::out_of_range("Source Index");
                }
                if (tindex > ba->Size()) {
                        throw std::out_of_range("Target Index");
                }
                size_t newLength = tindex + size;
                size_t capacity = ba->Capacity();
                if (newLength > capacity) {
                        while (newLength > capacity) {
                                capacity = capacity << 1;
                        }
                        ba->SetCapacity(capacity);
                }
                for (size_t i = 0; i < size; i++) {
                        (*this)[tindex + i] = qba[static_cast<int>(sindex + i)];
                }
                if (ba->Size() < (tindex + size)) {
                        ba->SetSize(tindex + size);
                }
        }

        /*! \brief Diese Methode `Dump()` schreibt die Daten dieser ByteArray-Instanz ganz oder teilweise in eine Datei.
         *
         * Die Methode prüft zunächst, ob die Parameter _ind_ und _lng_ innerhalb des zulässigen Wertebereiche liegen.
         * Wenn nicht, wird mit _false_ zurückgekehrt.
         * Die Methode versucht dann die Datei zum Schreiben zu öffnen.
         * Wenn dies nicht möglich ist, wird mit _false_ zurückgekehrt.
         * Enthält die QString-Instanz unter dem Parameter _title_ eine Überschrift, so wird diese zuerst in die Datei geschrieben.
         * Danach werden die Datenbytes aus dieser ByteArray-Instanz in die Datei geschrieben und die Datei wieder geschlossen.
         *
         * Nachfolgend ein Beispieldump, bei dem die ersten 27 Bytes aus einer ByteArray-Instanz verarbeitet wurden (__ind = 0__ und __lng = 27__).
         *
         *      Bierbestellung:
         *        decimal Index  hexIndex  Content(hexadezimal)                             Content(ASCII)
         *                    0  00000000  45 69 6e 20 47 6c 61 73 20 6b 61 6c 74 65 73 20  Ein Glas kaltes
         *                   16  00000010  42 69 65 72 20 62 69 74 74 65 21                 Bier bitte!
         *
         * \note Diese Methode ist nicht für den Wirkbetrieb gedacht. Sie kann aber bei Programmentwicklung manchmal sehr hilfreich sein.
         *
         * \param pathName Verweis auf die QString-Instanz, in der der Dateiname (ggf. mit Pfadangabe) steht.
         * \param title Verweis auf die QString-Instanz, in der der die Überschrift steht.
         * \param ind Der nullbasierte Quellindex in dieser ByteArray-Instanz, ab dem der Dump geschrieben werden soll.
         * \param lng Die Anzahl der zu dumpenden Bytes.
         * \return _true_ wenn kein Fehler aufgetreten ist, (sonst _false_).
         */
        bool ByteArray::Dump(const QString &pathName, const QString &title, const size_t ind, const size_t lng) {
                size_t length = ba->Size();
                if ((ind + lng) <= length) {
                        size_t position = ind;
                        if (lng == 0) {
                                length = length - position;
                        } else {
                                length = lng;
                        }
                        QFile sout(pathName);
                        if (!sout.open(QIODevice::ReadWrite)) {
                                return false;
                        }
                        size_t rest = length;
                        size_t index = 0;
                        ByteArray b(128);
                        unsigned char c;
                        if (sout.size() > 0) {
                                sout.seek(sout.size());
                        }
                        if (!title.isEmpty()) {
                                b.Append(title);
                                sout.write(&b[0], b.Size());
                        }
                        sout.write("  decimal Index  hexIndex  Content(hexadezimal)                             Content(ASCII)\r\n", 91);
                        while (rest > 15) {
                                b.Clear();
                                index = 0;
                                b.IntegerToText(index, static_cast<int>(position), 15, true);
                                b.Append(' ');
                                index++;
                                b.Append(' ');
                                index++;
                                b.IntegerToHexText(index, static_cast<int>(position), 8);
                                b.Append(' ');
                                index++;
                                b.Append(' ');
                                index++;
                                for (size_t i = 0; i < 16; i++) {
                                        c = static_cast<unsigned char>((*this)[position + i]);
                                        b.IntegerToHexText(index, static_cast<int>(c), 2);
                                        b.Append(' ');
                                        index++;
                                }
                                b.Append(' ');
                                for (size_t i = 0; i < 16; i++) {
                                        c = static_cast<unsigned char>((*this)[position + i]);
                                        if (c < 32) {
                                                b.Append('.');
                                        } else {
                                                if (c < 127) {
                                                        b.Append(c);
                                                } else {
                                                        b.Append('.');
                                                }
                                        }
                                        index++;
                                }
                                b.Append('\r');
                                b.Append('\n');
                                sout.write(&b[0], b.Size());
                                position = position + 16;
                                rest = rest - 16;
                        }
                        if (rest > 0) {
                                b.Clear();
                                index = 0;
                                b.IntegerToText(index, static_cast<int>(position), 15, true);
                                b.Append(' ');
                                index++;
                                b.Append(' ');
                                index++;
                                b.IntegerToHexText(index, static_cast<int>(position), 8);
                                b.Append(' ');
                                index++;
                                b.Append(' ');
                                index++;
                                for (size_t i = 0; i < rest; i++) {
                                        c = static_cast<unsigned char>((*this)[position + i]);
                                        b.IntegerToHexText(index, static_cast<int>(c), 2);
                                        b.Append(' ');
                                        index++;
                                }
                                for (size_t i = 0; i < (16 - rest); i++) {
                                        b.Append("   ");
                                }
                                b.Append(' ');
                                for (size_t i = 0; i < rest; i++) {
                                        c = static_cast<unsigned char>((*this)[position + i]);
                                        if (c < 32) {
                                                b.Append('.');
                                        } else {
                                                if (c < 127) {
                                                        b.Append(c);
                                                } else {
                                                        b.Append('.');
                                                }
                                        }
                                        index++;
                                }
                                b.Append('\r');
                                b.Append('\n');
                                sout.write(&b[0], b.Size());
                        }
                        sout.close();
                        return true;
                } else {
                        return false;
                }
        }

        /*! \brief Diese Methode `IndexOf()` gibt den nullbasierten Index des ersten vorhandenen Bytes aus der ByteArray-Instanz zurück.
         *
         * Es wird nach dem gewünschten Byte ab dem Anfang in der verwalteten Liste gesucht.
         * Falls es gefunden werden kann, wird sein nullbasierter Index zurückgegeben.
         * Wurde das Byte nicht gefunden, wird -1 zurückgegeben.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      int erg = ba->IndexOf('t');
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _erg_ den Wert __12__, da der erste Buchstabe __t__ an dieser Indexposition steht.
         * \param ref Enthält das gesuchte Byte.
         * \return Der nullbasierter Index oder -1, wenn das Byte nicht gefunden werden konnte.
         */
        int ByteArray::IndexOf(const char &ref) const {
                return ba->IndexOf(ref);
        }

        /*! \brief Diese Methode `Insert()` fügt ein Byte an der angegebenen (nullbasierten) Indexposition in die ByteArray-Instanz ein.
         *
         * Die Methode prüft zunächst, ob in der ByteArray-Instanz noch Platz vorhanden ist. Ist dies nicht der Fall, verdoppelt es zunächst die Kapazität der ByteArray-Instanz.
         * Die Methode prüft anschließend, ob der Indexwert innerhalb eines gültigen Wertebereichs liegt.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Falls ja, werden etwaige nachfolgende Bytes im Datenbereich um eine Position nach hinten verschoben, und das neue Byte wird an der gewünschten
         * Indexposition eingefügt.
         *
         *      ByteArray *ba = new ByteArray("EinGlas kaltes Bier bitte!");
         *      ba->Insert(3, ' ');
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 27 Bytes.
         * Davon sind 26 Bytes mit Daten belegt (der Bierbestellung mit einem Fehler).
         * Mit der Methode `Insert()` wird an der Indexposition 3 ein Leerzeichen eingefügt.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         * \param index Der Index in der Liste, an dem das Byte eingefügt werden soll.
         * \param ref Verweis auf das Byte, das in diese Array-Instanz eingefügt werden soll.
         */
        void ByteArray::Insert(const size_t index, const char &ref) {
                ba->Insert(index, ref);
        }

        /*! \brief Diese Methode `IntegerToHexText()` schreibt einen ganzzahligen Wert als Hexadezimaltext mit einer Länge von _length_ Zeichen in diese ByteArray-Instanz.
         *
         * Der eingefügte Text ist immer _length_ Zeichen lang und kann mit führenden Leerzeichen beginnen.
         * Vorhandene Inhalte werden überschrieben.
         * Wenn für die Eingabe zu wenig Platz zur Verfügung steht, werden nur die letzten Zeichen des Hexadezimaltextes geschrieben.
         * Die Variable _index_ wird um _length_ Bytes erhöht.
         * Diese Methode wird zum Beispiel von der Methode `Dump()` benutzt. Siehe dort die Beispielausgabe.
         *
         *      ByteArray *ba = new ByteArray();
         *      size_t index = 0;
         *      ba->IntegerToHexText(index, static_cast<int>('E'), 2);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 32 Bytes, von denen 0 Bytes belegt sind.
         * Die Variable _index_ wird auf den Wert __0__ gesetzt. Mit der Methode `IntegerToHexText()` wird dann die Hexadezimal-Codierung des Buchstaben __E__ an den
         * Anfang der ByteArray-Instanz geschrieben (2 Bytes). Die Variable _index_ hat nun den Wert __2__ und in der ByteArray-Instanz sind die ersten 2 Bytes mit
         * dem Text __45__ belegt.
         * \param index Verweis auf eine Variable vom Typ _size_t_ mit dem Startwert für den Schreibindex.
         * \param value Der ganzzahlige Wert (vom Typ „integer“), der in Text umgewandelt werden soll.
         * \param length Länge der gewünschten Textausgabe in Byte.
         * \sa Dump()
         */
        void ByteArray::IntegerToHexText(size_t &index, const int value, const size_t length) {
                QString s;
                s.setNum(value, 16);
                int size = s.size();
                if (size > static_cast<int>(length)) {
                        int diff = static_cast<int>(length) - size;
                        s.remove(0, diff);
                }
                while (s.size() < static_cast<int>(length)) {
                        s.insert(0, "0");
                }
                ByteArray b(s);
                this->Copy(b, 0, index, length);
                index = index + length;
        }

        /*! \brief Diese Methode `IntegerToText()` schreibt einen ganzzahligen Wert als Text mit einer Länge von _length_ Zeichen in diese ByteArray-Instanz.
         *
         * In den resultierenden Dezimalwert können Tausender-Trennpunkte eingefügt werden (Variable _points_ = true), der Standardwert ist jedoch _points_ = false.
         * Der eingefügte Text ist immer _length_ Zeichen lang und kann mit führenden Leerzeichen beginnen.
         * Vorhandene Inhalte werden überschrieben.
         * Wenn für den Eintrag zu wenig Speicherplatz bereitgestellt wird (_length_ zu klein gewählt), wird eine Ausnahme _std::out_of_range(„length“)_ ausgelöst.
         * Die Variable _index_ wird um _length_ Bytes erhöht.
         *
         *      ByteArray *ba = new ByteArray();
         *      size_t index = 0;
         *      ba->IntegerToText(index, 1234567, 10, true);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 32 Bytes, von denen 0 Bytes belegt sind.
         * Die Variable _index_ wird auf den Wert __0__ gesetzt. Mit der Methode `IntegerToText()` wird dann die Text-Codierung des Integer-Wertes __1234567__
         * mit eingefügten Tausender-Punkten an den
         * Anfang der ByteArray-Instanz geschrieben (10 Bytes). Die Variable _index_ hat nun den Wert __10__ und in der ByteArray-Instanz sind die ersten 10 Bytes mit
         * dem Text: zwei Leerzeichen "  " und dann __1.234.567__ belegt.
         * \param index Verweis auf eine Variable vom Typ _size_t_ mit dem Startwert für den Schreibindex.
         * \param value Der ganzzahlige Wert (vom Typ „integer“), der in Text umgewandelt werden soll.
         * \param length Länge der gewünschten Textausgabe in Byte.
         * \param points Wenn _true_, werden Tausender-Trennpunkte eingefügt (der Standardwert ist _false_).
         */
        void ByteArray::IntegerToText(size_t &index, const int value, const size_t length, const bool points) {
                QString s;
                s.setNum(value);
                if (points) {
                        size_t length1 = s.size();
                        size_t length2;
                        if (s[0] == '-') {
                                length2 = length1 - 1;
                        } else {
                                length2 = length1;
                        }
                        if (length2 > 3) {
                                s.insert(static_cast<int>(length1) - 3, '.');
                        }
                        if (length2 > 6) {
                                s.insert(static_cast<int>(length1) - 6, '.');
                        }
                        if (length2 > 9) {
                                s.insert(static_cast<int>(length1) - 9, '.');
                        }
                        if (length2 > 12) {
                                s.insert(static_cast<int>(length1) - 12, '.');
                        }
                        if (length2 > 15) {
                                s.insert(static_cast<int>(length1) - 15, '.');
                        }
                        if (length2 > 18) {
                                s.insert(static_cast<int>(length1) - 18, '.');
                        }
                }
                while (s.size() < static_cast<int>(length)) {
                        s.insert(0, " ");
                }
                if (s.size() > static_cast<int>(length)) {
                        throw std::out_of_range("length");
                }
                ByteArray b(s);
                this->Copy(b, 0, index, length);
                index = index + length;
        }

        /*! \brief Diese Methode `IsEqual()` vergleicht einen Datenbereich der übergebenen ByteArray-Instanz mit einen Datenbereich dieser ByteArray-Instanz.
         *
         * Die Methode vergleicht einen beliebigen Datenbereich aus ihrer eigenen Instanz mit einem Datenbereich aus einer übergebenen Instanz.
         * Bei der ersten Ungleichheit von zwei Bytes wird die Prüfung abgebrochen und _false_ zurückgegeben.
         * Nur wenn beide Datenbereiche vollständig übereinstimmen, wird _true_ zurückgegeben.
         *
         *      ByteArray *ba1 = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      ByteArray *ba2 = new ByteArray("Zwei Glas kaltes Bier bitte!");
         *      bool equal = ba1->IsEqual(*ba2, 5, 4, 16);
         *      delete ba1;
         *      delete ba2;
         *
         * Die Variable _equal_ hat in diesem Beispiel den Wert __true__.
         * In der ersten ByteArray-Instanz (Ein __Glas kaltes Bier__ bitte!) und der zweiten ByteArray-Instanz (Zwei __Glas kaltes Bier__ bitte!)
         * wurden die __fett__ markierten Bereiche miteinander verglichen.
         * \note Da beide Datenbereiche nur gelesen werden, wird nicht geprüft, ob sie sich auch innerhalb der Bereiche der verwalteten Arrays befinden.
         *
         * \param ref Die Referenz auf die ByteArray-Instanz, mit der verglichen werden soll.
         * \param sindex Der nullbasierte Index in der übergebenen ByteArray-Instanz, ab dem der Vergleich beginnen soll.
         * \param tindex Der nullbasierte Index in dieser ByteArray-Instanz, ab dem der Vergleich beginnen soll.
         * \param size Die Anzahl der zu vergleichenden Bytes.
         * \return _true_, wenn beide Bytebereiche gleich sind (ansonsten _false_).
         * \sa Compare()
         */
        bool ByteArray::IsEqual(const ByteArray &ref, const size_t sindex, const size_t tindex, const size_t size) const {
                return ba->IsEqual(*(ref.ba), sindex, tindex, size);
        }

        /*! \brief Diese Methode `ReadNumber()` liest einen Integer-Wert aus der ByteArray-Instanz.
         *
         * Die Methode prüft zunächst, ob der referenzierte Indexwert auf einen gültigen Datenbereich in der ByteArray-Instanz verweist.
         * Wenn nicht, wird eine Ausnahme (_out_of_range(„Index“)_) ausgelöst.
         * Dann wird geprüft ob der Parameter _bytes_ kleiner oder gleich sizeof(int) ist.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("bytes")_) ausgelöst.
         * Die Methode liest dann einen Integer-Wert (der Parameter _bytes_ lang ist) an der übergebenen Indexposition aus dem Byte-Array aus,
         * je nach dem Parameter (bool _littelEndian_) entweder im little-endian-Format oder im big-endian-Format (little-endian ist dabei das Standardformat)
         * und erhöht den referenzierten Indexwert anschließend.
         * Je nach dem Parameter _sign_ wird der Wert mit oder ohne Vorzeichen zurückgegeben (mit Vorzeichen ist der Standardwert).
         *
         *      ByteArray *ba = new ByteArray("\002\001\0");
         *      size_t index = 0;
         *      int erg = ba->ReadNumber(index, 3);
         *      delete ba;
         *
         * Die Methode `ReadNumber()` liest in diesem Beispiel den Integerwert im little-endian Format aus der BytaArray-Instanz. Die Variable _index_
         * enthält danach den Wert __3__. Die Variable _erg_ enthält den Wert __258__.
         * \param index Verweis auf einen Integer-Wert, der den Startindex des Lesevorgangs enthält.
         * \param bytes Die Breite des Integerwertes in Bytes.
         * \param littleEndian _true_, wenn der Ganzzahlwert im little-endian-Format geschrieben werden soll (sonst _false_).
         * \param sign _true_, wenn der Integer-Wert mit Vorzeichen zurückgegeben werden soll (ansonsten _false_).
         * \return Den gelesenen Integer-Wert.
         * \sa WriteNumber()
         */
        int ByteArray::ReadNumber(size_t &index, const size_t bytes, const bool littleEndian, const bool sign) const {
                int result = 0;
                size_t size = ba->Size();
                if (index > (size - bytes)) {
                        throw std::out_of_range("Index");
                }
                if (bytes > sizeof(int)) {
                        throw std::out_of_range("bytes");
                }
                int temp;
                unsigned char c1;
                unsigned char c2;
                if (littleEndian) {
                        c1 = 0;
                        for (size_t i = 0; i < bytes; i++) {
                                c1 = (*ba)[index];
                                temp = c1 << (i * 8);
                                result = result + temp;
                                index++;
                        }
                } else {
                        c1 = (*ba)[index];
                        for (int i = static_cast<int>(bytes) - 1; i >= 0; i--) {
                                c2 = (*ba)[index];
                                temp = c2 << (i * 8);
                                result = result + temp;
                                index++;
                        }
                }
                if (sign) {
                        if ((bytes < sizeof(int)) && (c1 > 127)) {
                                temp = 1 << (bytes * 8);
                                result = result - temp;
                        }
                }
                return result;
        }

        /*! \brief Die Methode `ReadX209Int64()` liest einen Integerwert mit dem in X.209 beschriebenen Verfahren aus der ByteArray-Instanz.
         *
         * Die Methode `ReadX209Int64()` liest einen Integerwert aus der ByteArray-Instanz ab dem im Parameter _index_ angegebenen Indexwert.
         * Nach dem Lesevorgang wird der Parameter _index_ angepasst.
         *
         *      ByteArray *ba = new ByteArray("\202\002");
         *      size_t index = 0;
         *      int erg = ba->ReadX209Int64(index);
         *      delete ba;
         *
         * Die Methode `ReadX209Int64()` liest in diesem Beispiel den Integerwert im little-endian Format aus der BytaArray-Instanz. Die Variable _index_
         * enthält danach den Wert __2__. Die Variable _erg_ enthält den Wert __258__.
         *  \param index Verweis auf einen Integer-Wert, der den Startindex des Lesevorgangs enthält.
         *  \param reverse _true_, wenn der Integerwert in Richtung kleiner Indexwerte geschrieben werden soll (der Standardwert ist _false_).
         *  \sa WriteX209Int64()
         */
        int64_t ByteArray::ReadX209Int64(size_t &index, const bool reverse) {
                int64_t result = 0;
                size_t i = 0;
                int64_t temp;
                unsigned char c = (*ba)[index];
                if (reverse) {
                        index--;
                } else {
                        index++;
                }
                while (c >= 0x80) {
                        c = c & 0x7F;
                        temp = c;
                        temp = temp << (i * 7);
                        result = result + temp;
                        i++;
                        c = (*ba)[index];
                        if (reverse) {
                                index--;
                        } else {
                                index++;
                        }
                }
                if (i > 8) {
                        if (c > 0) {
                                temp = 0X8000000000000000;
                                result = result + temp;
                        }
                } else {
                        temp = c;
                        temp = temp << (i * 7);
                        result = result + temp;
                        if (c >= 0x40) {
                                temp = -1;
                                temp = temp << ((i + 1) * 7);
                                result = result + temp;
                        }
                }
                return result;
        }

        /*! \brief Diese Methode `RemoveAt()` löscht ein Byte an einer bestimmten Indexposition aus der ByteArray-Instanz.
         *
         * Die Methode prüft zunächst, ob der Indexwert innerhalb eines gültigen Wertebereichs liegt (verweist auf ein Byte, das bereits verwaltet wird).
         * Wenn nicht, wird eine Ausnahme (_out_of_range("Index")_) ausgelöst.
         * Anschließend werden, falls erforderlich, nachfolgende Bytes im Datenfeld an der Indexposition vorgezogen, und die Variable _size_ wird um eins verringert.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas  kaltes Bier bitte!");
         *      ba->RemoveAt(8);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 29 Bytes.
         * Davon sind 28 Bytes mit Daten belegt (der Bierbestellung mit einem überflüssigem Leerzeichen).
         * Mit der Methode `RemoveAt(8)` wird dieses entfernt und die nachfolgenden Bytes um eine Indexposition verschoben.
         * In der ByteArray-Instanz sind nun 27 Bytes mit Daten belegt.
         * Vor der Zerstörung der ByteArray-Instanz, wird der Heapspeicher in diesem Beispiel __nicht__ gelöscht.
         * \param index Der Index im Datenfeld, in dem das Byte gelöscht werden soll.
         */
        void ByteArray::RemoveAt(const size_t index) {
                ba->RemoveAt(index);
        }

        /*! \brief Diese Methode `SetCapacity()` ändert die Kapazität (die Anzahl der verwaltbaren Bytes) in der ByteArray-Instanz.
         *
         * Die Methode prüft zunächst, ob der übergebene Kapazitätswert größer ist als die Anzahl der derzeit verwalteten Bytes.
         * Wenn nicht, wird eine Ausnahme (_out_of_range("size")_) ausgelöst.
         * Anschließend wird neuer Speicher (mit der gewünschten Kapazität) aus dem Heap zugewiesen, die bereits verwalteten Bytes werden in den neuen Speicher
         * kopiert, und der alte Speicherplatz auf dem Heap wird wieder freigegeben.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      ba->SetCapacity(128);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 28 Bytes.
         * Davon sind 27 Bytes mit Daten belegt (der Bierbestellung).
         * Mit der Methode `SetCapacity(128)` wird die Kapazität der ByteArray-Instanz auf 128 erhöht. Die Daten bleiben dabei erhalten.
         * \param size Die neue Kapazitätsgröße der ByteArray-Instanz.
         * \sa TestCapacity() Capacity() Size() SetSize()
         */
        void ByteArray::SetCapacity(const size_t size) {
                ba->SetCapacity(size);
        }

        /*! \brief Diese Methode `SetSize()` setzt die Anzahl der verwalteten Bytes in der ByteArray-Instanz auf einen neuen Wert.
         *
         * Die Methode prüft zunächst, ob der übergebene Wert größer ist als die aktuelle Kapazität.
         * Wenn ja, wird eine Ausnahme (_out_of_range("size")_) ausgelöst.
         * Anschließend wird die Variable _size_ auf den übergebenen Wert gesetzt.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      ba->SetSize(26);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 28 Bytes.
         * Davon sind 27 Bytes mit Daten belegt (der Bierbestellung).
         * Mit der Methode `SetSize(26)` werden nun nur noch 26 Byte Daten verwaltet (das Ausrufezeichen nicht mehr). Die Daten bleiben aber erhalten.
         * \param size Die neue Anzahl der verwalteten Bytes.
         * \sa Size() Capacity() SetCapacity() TestCapacity()
         */
        void ByteArray::SetSize(const size_t size) {
                ba->SetSize(size);
        }

        /*! \brief Diese Methode `TestCapacity()` prüft, ob die Kapazität (die Anzahl der verwaltbaren Bytes) in der ByteArray-Instanz mindestens den erforderlichen Mindestwert aufweist.
         *
         * Wenn die aktuelle Kapazität der Array-Instanz unter dem erforderlichen Mindestwert liegt, wird sie erhöht.
         * Die Kapazität kann mit dieser Methode nicht verkleinert werden.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      ba->TestCapacity(128);
         *      delete ba;
         *
         * In diesem Beispiel enthält die Variable _ba_ einen Zeiger auf die ByteArray-Instanz. Diese hat eine Kapazität von 28 Bytes.
         * Davon sind 27 Bytes mit Daten belegt (der Bierbestellung).
         * Mit der Methode `TestCapacity(128)` wird die Kapazität der ByteArray-Instanz auf 128 erhöht. Die Daten bleiben dabei erhalten.
         * \param size Die erforderliche Mindestkapazität der ByteArray-Instanz.
         * \sa SetCapacity() Capacity() Size() SetSize()
         */
        void ByteArray::TestCapacity(const size_t size) {
                ba->TestCapacity(size);
        }

        /*! \brief Diese Methode `ToInteger()` wandelt den Inhalt dieser ByteArray-Instanz in einen Integer- Wert um.
         *
         * Kann die Konvertierung nicht erfolgreich durchgeführt werden, wird der Integer-Wert 0 zurückgegeben
         * und eine per Zeiger übergebene boolesche Variable auf _false_ gesetzt (im Erfolgsfall auf _true_).
         * Als Basiswerte für die Umrechnung können 2, 8, 10 oder 16 gewählt werden.
         *
         *      ByteArray *ba = new ByteArray("-4526");
         *      int erg = ba->ToInteger();              // Variable erg = -4526
         *      delete ba;
         *
         *      ByteArray *ba = new ByteArray("011001110");
         *      bool ok = false;
         *      int erg = ba->ToInteger(&ok, 2);        // Variable erg = 206 und ok = true
         *      delete ba;
         *
         * \param ok Zeiger auf eine boolesche Variable (der Standardwert ist ein _nullptr_).
         * \param base Basiswert, mit dem die Umrechnung durchgeführt wird (der Standardwert ist 10).
         * \return Der berechnete Integer-Wert.
         */
        int ByteArray::ToInteger(bool *ok, const int base) {
                int result = 0;
                QString *s = this->ToQString();
                result = s->toInt(ok, base);
                delete s;
                return result;
        }

        /*! \brief Diese Methode `ToQString()` konvertiert einen UTF-8-Inhalt in dieser ByteArray-Instanz in eine QString-Instanz.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      QString *qs = ba->ToQString();
         *      delete ba;
         *      delete qs;
         *
         * Die Methode `ToQString()` erzeugt eine neue QString-Instanz und kopiert den Inhalt aus der ByteArray-Instanz dort hinein.
         * Auch die erzeugte QString-Instanz muss irgendwann wieder freigegeben werden.
         * \return Zeiger auf eine QString-Instanz.
         */
        QString *ByteArray::ToQString() const {
                QString *s = new QString;
                QByteArray qba(&(*ba)[0], static_cast<int>(ba->Size()));
                s->append(QString::fromUtf8(qba));
                return s;
        }

        /*! \brief Die Methode `ToString()` konvertiert den UTF-8-Inhalt dieser ByteArray-Instanz in eine Standardzeichenkette um.
         *
         *      ByteArray *ba = new ByteArray("Ein Glas kaltes Bier bitte!");
         *      std::string *str = ba->ToString();
         *      delete ba;
         *      delete str;
         *
         * Die Methode `ToString()` erzeugt eine neue Standard-String-Instanz und kopiert den Inhalt aus der ByteArray-Instanz dort hinein.
         * Auch die erzeugte Standard-String-Instanz muss irgendwann wieder freigegeben werden.
         * \return Zeiger auf eine Zeichenkette vom Typ _std::string_.
         */
        std::string *ByteArray::ToString() const {
                std::string *s = new std::string("");
                size_t length = Size();
                if (length > 0) {
                        s->append(&(*ba)[0], length);
                }
                return s;
        }

        /*! \brief Die Methode `WriteNumber()` schreibt einen Integer-Wert in die ByteArray-Instanz.
         *
         *  Die Methode prüft zunächst, ob der referenzierte Indexwert auf einen gültigen Bytebereich in der ByteArray-Instanz verweist.
         *  Wenn nein, wird eine Ausnahme (_out_of_range("Index")_) geworfen. Dann wird geprüft ob der Parameter _bytes_ kleiner oder gleich
         *  sizeof(int) ist. Wenn nein, wird eine Ausnahme (_out_of_range("bytes")_) geworfen.
         *  Die Methode schreibt dann einen (Parameter _bytes_ langen) Integerwert an die übergebenen Indexposition in das ByteArray,
         *  je nach dem Parameter (bool _littelEndian_) entweder im little-endian-Format oder im big-endian-Format (little-endian ist dabei das Standardformat)
         *  und erhöht den referenzierten Indexwert anschließend.
         *
         *      ByteArray *ba = new ByteArray();
         *      size_t index = 0;
         *      ba->WriteNumber(index, 258, 4);
         *      char erg = (*ba)[0];            // erg = 0x02
         *      char erg = (*ba)[1];            // erg = 0x01
         *      char erg = (*ba)[2];            // erg = 0x00
         *      char erg = (*ba)[3];            // erg = 0x00
         *      delete ba;
         *
         * Die Methode schreibt in diesem Beispiel den übergebenen Integerwert (258) im little-endian Format in die BytaArray-Instanz. Die Variable _index_
         * enthält danach den Wert __4__.
         *  \param index Referenz auf eine Variable vom Type _size_t_ mit dem Beginn für den Schreibindex.
         *  \param value Den zu schreibenden Integerwert.
         *  \param bytes Die Breite des Integerwertes in Bytes.
         *  \param littleEndian true, wenn der Ganzzahlwert im little-endian-Format geschrieben werden soll (sonst false).
         *  \sa ReadNumber()
         */
        void ByteArray::WriteNumber(size_t &index, const int value, const size_t bytes, const bool littleEndian) {
                if (index > ba->Size()) {
                        throw std::out_of_range("Index");
                }
                if (bytes > sizeof(int)) {
                        throw std::out_of_range("bytes");
                }
                size_t capacity = ba->Capacity();
                if (index > (capacity - bytes)) {
                        capacity = capacity << 1;
                        ba->SetCapacity(capacity);
                }
                if (littleEndian) {
                        for (size_t i = 0; i < bytes; i++) {
                                int temp = value >> (i * 8);
                                unsigned char c = temp & 0xFF;
                                (*ba)[index] = c;
                                index++;
                        }
                } else {
                        for (int i = static_cast<int>(bytes) - 1; i >= 0; i--) {
                                int temp = value >> (i * 8);
                                unsigned char c = temp & 0xFF;
                                (*ba)[index] = c;
                                index++;
                        }
                }
                size_t size = ba->Size();
                if (index > size) {
                        ba->SetSize(index);
                }
        }

        /*! \brief Die Methode `WriteX209Int64()` schreibt einen Integer-Wert mit dem in X.209 beschriebenen Verfahren in die ByteArray-Instanz.
         *
         *  Die Methode prüft zunächst, ob der referenzierte Indexwert auf einen gültigen Bytebereich in der ByteArray-Instanz verweist.
         *  Wenn nein, wird eine Ausnahme (_out_of_range("Index")_) geworfen.
         *  Danach wird geprüft, ob in der ByteArray-Instanz noch genügend Platz für den einzutragenden Integerwert ist.
         *  Wenn nein, wird die Größe der ByteArray-Instanz zunächst verdoppelt. Ist der Parameter -reverse_ dabei auf _true_ gesetzt, so wird
         *  der neue zusätzliche Speicherplatz am Anfang der ByteArray-Instanz bereitgestellt und der Parameter _index_ entsprechend berichtigt.
         *  Zum Schluss wird der neue Integerwert in die ByteArray-Instanz geschrieben und der Parameter _index_ angepasst.
         *
         *      ByteArray *ba = new ByteArray();
         *      size_t index = 0;
         *      ba->WriteX209Int64(index, 258);
         *      char erg = (*ba)[0];            // erg = 0x82
         *      char erg = (*ba)[1];            // erg = 0x02
         *      delete ba;
         *
         * Die Methode schreibt in diesem Beispiel den übergebenen Integerwert (258) im little-endian Format in die BytaArray-Instanz. Die Variable _index_
         * enthält danach den Wert __2__.
         *  \param index Referenz auf eine Variable vom Type _size_t_ mit dem Beginn für den Schreibindex.
         *  \param value Den zu schreibenden Integerwert (bis zu 64 Bit lang).
         *  \param reverse _true_, wenn der Integerwert in Richtung kleiner Indexwerte geschrieben werden soll (der Standardwert ist _false_).
         *  \sa ReadX209Int64()
         */
        void ByteArray::WriteX209Int64(size_t &index, const int64_t value, const bool reverse) {
                size_t size = ba->Size();
                if (index > size) {
                        throw std::out_of_range("Index");
                }
                size_t k = 0;
                size_t temp = 0;
                unsigned char c;
                if (value < 0) {
                        for (int i = 8; i >= 0; i--) {
                                temp = (value >> (i * 7)) & 0x7F;
                                if (temp < 0x7F) {
                                        k = i;
                                        break;
                                }
                        }
                        if (!(temp >= 0x40)) {
                                k++;
                        }
                } else {
                        for (int i = 8; i >= 0; i--) {
                                temp = (value >> (i * 7)) & 0x7F;
                                if (temp > 0) {
                                        k = i;
                                        break;
                                }
                        }
                        if (temp >= 0x40) {
                                k++;
                        }
                }
                size_t clength = ba->Capacity();
                if (reverse) {
                        if (index < (k + 1)) {
                                size_t newcapacity = clength << 1;
                                ba->SetCapacity(newcapacity);
                                for (size_t i = 0; i < clength; i++) {
                                        (*this)[clength + i] = (*this)[i];
                                }
                                size = size + clength;
                                index = index + clength;
                        }
                } else {
                        if (index > (clength - k - 1)) {
                                size_t newcapacity = clength << 1;
                                ba->SetCapacity(newcapacity);
                        }
                }
                for (size_t j = 0; j < k; j++) {
                        temp = value >> (j * 7) & 0x7F;
                        c = temp + 0x80;
                        (*ba)[index] = c;
                        if (reverse) {
                                index--;
                        } else {
                                index++;
                        }
                }
                temp = value >> (k * 7) & 0x7F;
                c = temp;
                (*ba)[index] = c;
                if (reverse) {
                        index--;
                } else {
                        index++;
                }
                if (!reverse) {
                        if (index > size) {
                                ba->SetSize(index);
                        }
                }
        }

        /*! \brief Die Methode `WriteareaReference()` garantiert die gewünschte Feldgröße und gibt eine Referenz (zum Schreiben) in die ByteArray-Instanz zurück.
         *
         * Die Methode prüft zunächst, ob die Kapazität im verwalteten Datenbereich für den Schreibvorgang ausreicht.
         * Ist dies nicht der Fall, wird die Kapazität erhöht (zumindest verdoppelt), bis sie ausreicht.
         * Ist der Schreibindex am Ende des Vorgangs größer als die Variable _size_, wird _size_ auf das Schreibende gesetzt.
         *
         * \param index Nullbasierter Schreibindex im Datenbereich, in dem geschrieben werden soll.
         * \param size Anzahl der zu schreibenden Bytes.
         * \return Verweis auf die Addresse im Datenbereich, ab der geschrieben werden soll.
         */
        char &ByteArray::WriteareaReferenz(const size_t index, const size_t size) {
                return ba->WriteareaReferenz(index, size);
        }

} // end of namespace RH

