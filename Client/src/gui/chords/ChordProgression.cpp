#include "ChordProgression.h"
#include <QRegularExpression>
#include "BpiUtils.h"

//+++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++
const QString Chord::TABLE_SHARPS[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const QString Chord::TABLE_FLATS[] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
//++++++++++++++++++++++++++++++++++++++++++++++++++

Chord::Chord(QString chordName) {
    initialize(chordName, 0);
}

Chord::Chord(QString chordText, int beat) {
    initialize(chordText, beat);
}

void Chord::initialize(QString chordText, int beat){
    this->chordText = chordText.trimmed();
    this->beat = beat;
    int indexOfInversionBar = this->chordText.indexOf("/");
    if (indexOfInversionBar > 0) {
        bassInversion = this->chordText.right(this->chordText.size() - 1 - indexOfInversionBar);
    }
    hasLettersAfterChordRoot = false;//min, maj, m, dim, add letters after chord root?
    int lettersBegin = (isFlat() || isSharp()) ? 2 : 1;
    int limit = hasBassInversion() ? indexOfInversionBar : this->chordText.length();
    int endOfLetters = lettersBegin;
    while (endOfLetters < limit && !(chordText.at(endOfLetters).isDigit() || chordText.at(endOfLetters) == 'º' || chordText.at(endOfLetters) == '(')) {
        endOfLetters++;
        hasLettersAfterChordRoot = true;
    }
    if (hasLettersAfterChordRoot) {
        lettersAfterRoot = this->chordText.mid(lettersBegin, endOfLetters - lettersBegin);
    }

    if (endOfLetters < limit) {
        int finalIndex = hasBassInversion() ? indexOfInversionBar : this->chordText.length();
        lastPart = this->chordText.mid(endOfLetters, finalIndex - endOfLetters);
    }
}


QString Chord::getRootKey() const{
    int lenght =(isFlat() || isSharp()) ? 2 : 1;
    return chordText.left(lenght);
}

bool Chord::isSharp() const{
    return chordText.length() > 1 && chordText.at(1) == '#';
}

bool Chord::isFlat() const{
    return chordText.length() > 1 && chordText.at(1) == 'b';
}

Chord Chord::getTransposedVersion(int semitones) const{
    if (semitones == 0) {
        return Chord(getChordText(), getBeat());
    }
    QString newRootKey = getTransposedRoot(getRootKey(), semitones);
    QString newChordText = newRootKey;
    if(hasLettersAfterRoot()){
        newChordText += getLettersAfterRoot();
    }
    if(hasLastPart()){
        newChordText += getLastPart();
    }
    if(hasBassInversion()){
        QString newBassInversionRoot = getTransposedRoot( getBassInversion(), semitones);
        newChordText += "/" + newBassInversionRoot;
    }
    return Chord(newChordText, getBeat());
}

QString Chord::getTransposedRoot(QString rootKey, int semitones){
    QString newRootKey = "";
    int entryIndex = 0;
    const QString* table = getTable(rootKey, semitones);
    while (entryIndex < 12 && !(rootKey == table[entryIndex])) {
        entryIndex++;
    }
    if (entryIndex >= 12) {
        throw std::runtime_error( "rootKey not founded in upTable: " + rootKey.toStdString() );
    }
    if (semitones > 0) {
        newRootKey = table[(entryIndex + semitones) % 12];
    } else {
        newRootKey = table[(entryIndex + 12 + semitones) % 12];
    }
    return newRootKey;
}


const QString* Chord::getTable(QString chordName, int semitones) {
    bool isSharp = chordName.length() > 1 && chordName.at(1) == '#';
    bool isFlat = chordName.length() > 1 && chordName.at(1) == 'b';
    //boolean isNaturalKey = !isBemol && !isSustenido;
    if (semitones > 0) {
        return (isFlat) ? TABLE_FLATS : TABLE_SHARPS;
    }
    return (isSharp) ? TABLE_SHARPS : TABLE_FLATS;
}
//+++++++++++++++++++++++++++++++
bool operator== (const Chord& c1, const Chord &c2)
{
    if (c1.getChordText() != c2.getChordText()) {
        return false;
    }
    if (c1.getBeat() != c2.getBeat()) {
        return false;
    }
    return true;
}
//++++++++++++++++++++++++++++++++++++++++++

ChordProgressionMeasure::ChordProgressionMeasure(int beatsInTheMeasure)
    :beats(beatsInTheMeasure){

}

void ChordProgressionMeasure::addChord(Chord chord, int beat){
    if(beat < 0 || beat >= this->beats){
        throw std::runtime_error("invalid beat " + beat);
    }
    chords.insert(beat, chord);
}

QString ChordProgressionMeasure::toString() const{
    QString str = "| ";
    for (Chord chord : chords) {
        str += chord.getChordText() + "    ";
    }
    return str;
}


//++++++++++++++++++++

ChordProgression::ChordProgression()
{

}

bool ChordProgression::canBeUsed(int bpi) const{
    QList<int> dividers = BpiUtils::getBpiDividers(bpi);
    for (int divider : dividers) {
        if (divider == measures.size()) {
            return true;
        }
    }
    return false;
}

void ChordProgression::addMeasure(ChordProgressionMeasure measure){
    measures.append(measure);
}

ChordProgression ChordProgression::getStretchedVersion(int bpi) {
    if (!canBeUsed(bpi)) {
        throw std::runtime_error("This chord progression can't be used in bpi " + bpi);
    }
    int newbeatsPerMesure = bpi / measures.size();

    ChordProgression stretchedProgression;
    for (ChordProgressionMeasure originalMeasure : measures) {
        ChordProgressionMeasure newMeasure(newbeatsPerMesure);
        foreach (Chord chord , originalMeasure.getChords()) {
            int newChordBeat = chord.getBeat() * newbeatsPerMesure;
            newMeasure.addChord(Chord(chord.getChordText(), newChordBeat), newChordBeat);
        }
        stretchedProgression.addMeasure(newMeasure);
    }

    return stretchedProgression;
}

ChordProgression ChordProgression::getTransposedVersion(int semitones) {
    ChordProgression newProgression;
    for (ChordProgressionMeasure originalMeasure : measures) {
        ChordProgressionMeasure newMeasure(originalMeasure.getBeats());
        foreach (Chord chord , originalMeasure.getChords()) {
            newMeasure.addChord(chord.getTransposedVersion(semitones), chord.getBeat());
        }
        newProgression.addMeasure(newMeasure);
    }
    return newProgression;
}

int ChordProgression::getBeatsPerInterval() const{
    if(measures.isEmpty()){
        return 0;
    }
    int beatsPerMeasure = measures.first().getBeats();
    return beatsPerMeasure * getMeasures().size();
}

QString ChordProgression::toString() const{
    QString string;
    foreach (ChordProgressionMeasure measure , measures) {
        string += measure.toString();
    }
    return string;
}

void ChordProgression::clear(){
    measures.clear();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const QString ChatChordsProgressionParser::CHORD_REGEX = "([A-G][b#]?([ a-z0-9º()#\\+\\-]{1,8})?(/[A-G][b#]?[ ]{0,3})?)";
const QString ChatChordsProgressionParser::MEASURE_SEPARATORS_REGEX = "\\||!|I|l|L";

bool ChatChordsProgressionParser::containsProgression(QString string){
    QRegularExpression regex("^([" + MEASURE_SEPARATORS_REGEX + "][ ]{0,2}" + CHORD_REGEX + "{1,4}){1,}");
    QString cleanedString = getSanitizedString(string);
    QRegularExpressionMatch match = regex.match(cleanedString);
    return match.hasMatch();
}

ChordProgression ChatChordsProgressionParser::parse(QString string){
    //remove the chord separator in the end of string. Some users type this last separator
    QString cleanedString = getSanitizedString(string);

    if (containsProgression(cleanedString)) {
        ChordProgression progression;
        QStringList measuresStrings = cleanedString.split(QRegularExpression( MEASURE_SEPARATORS_REGEX));
        QRegularExpression matcher(CHORD_REGEX);
        for (QString measureString : measuresStrings) {
            ChordProgressionMeasure measure(4);//use 4 beats as default
            int chordsFoundedInTheMeasure = 0;
            QRegularExpressionMatchIterator i = matcher.globalMatch(measureString);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                QString chordName = match.captured();

                int chordBeat = chordsFoundedInTheMeasure++;
                measure.addChord( Chord(chordName, chordBeat), chordBeat);
            }
            if (chordsFoundedInTheMeasure > 0) {
                progression.addMeasure(measure);
            }
        }
        return progression;
    }
    return ChordProgression();//empty progression
}

QString ChatChordsProgressionParser::getSanitizedString(QString string) {
    //check if user type a measure separator or blank space in the end of string
    int index = string.size() - 1;
    QRegularExpression matcher(MEASURE_SEPARATORS_REGEX + "| ");//including black space in regex
    while(matcher.match(string.mid(index, 1)).hasMatch()){
        index--;
    }
    return string.left(index + 1);
}
