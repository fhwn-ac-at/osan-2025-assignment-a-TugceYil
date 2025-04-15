[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/H1vNwaly)
# Protokoll

Dieses Kapitel beschreibt, das Ziel eines Protokolls.


1. Einleitung
   
Dieses Protokoll soll darstellen, was im Rahmen der Lehrveranstaltung Grundlagen Betriebssysteme und Netzwerke behandelt wurde, und beschreibt dabei die einzelnen Schritte, die unternommen wurden, um die Aufgabe umzusetzen, einen Task Ventilator zu entwickeln, der Aufgaben an mehrere Worker-Prozesse verteilt, deren Ausführung koordiniert, Ergebnisse einsammelt und den geordneten Abschluss aller Prozesse sicherstellt.

2. Ziel der Aufgabe
   
Ziel war die Entwicklung eines einfachen verteilten Systems, bestehend aus einem sogenannten „Task Ventilator“ und mehreren Worker-Prozessen. Der Ventilator verteilt Aufgaben über eine Message Queue an die Worker, die diese Aufgaben anschließend abarbeiten. Jede Aufgabe besteht aus einem Integer-Wert, welcher die Anzahl der Sekunden angibt, die der Worker zur Abarbeitung (simuliert durch sleep) benötigt. Nach Erledigung sendet der Worker eine Rückmeldung an den Ventilator. Bei Erhalt eines speziellen Terminierungstasks (Wert 0) beendet sich der Worker nach Rückmeldung.

3. Technische Umsetzung
   
Verwendung von fork() zur Erstellung der Worker-Prozesse
Einsatz von System V Message Queues zur Interprozesskommunikation

Kommunikation erfolgt in zwei Richtungen:

•	Ventilator → Worker (Aufgabenverteilung)

•	Worker → Ventilator (Ergebnismeldung)

Jeder Worker verwaltet intern:

•	Anzahl verarbeiteter Tasks

•	Gesamtzeit, die er geschlafen hat

Am Ende sendet jeder Worker eine Abschlussnachricht mit PID, Anzahl Aufgaben und Schlafdauer


4. Lerninhalte & Erkenntnisse
   
•	Verständnis für parallele Prozessstrukturen: Die Rolle von Parent- und Child-Prozessen sowie deren Kommunikation wurde durch die praktische Umsetzung deutlich.

•	Interprozesskommunikation (IPC): Message Queues bieten eine strukturierte und sichere Möglichkeit, Daten zwischen Prozessen auszutauschen.

•	Ressourcenmanagement: Notwendigkeit zur richtigen Initialisierung und Freigabe von Queues, insbesondere beim Terminieren von Prozessen.

•	Strukturierung mittels Header-Files: Ein zentraler Aspekt zur Trennung von Deklaration und Implementierung (wird auch in späteren Übungen wieder relevant).


5. Herausforderungen
   
Da einige Teile der Aufgabe unter Zeitdruck („Speed-Coding“) behandelt wurden, war die Nachvollziehbarkeit anfangs erschwert.
Das Zusammenspiel aus Queue-Typen, Nachrichtentypen und synchronem Empfang musste zusätzlich recherchiert werden.
Die Logik zur sauberen Beendigung aller Prozesse erforderte besonderes Augenmerk, um keine  falschen-Prozesse zu erzeugen.

6. Fazit
   
Die Übung vermittelte grundlegende Konzepte der parallelen Verarbeitung und Interprozesskommunikation.
Durch die Verteilung und Koordination von Aufgaben zwischen mehreren Prozessen wurde das Verständnis für typische Mechanismen moderner Betriebssysteme vertieft. 
Die klare Trennung zwischen steuernder Instanz und ausführenden Einheiten zeigte zudem, wie sich solche Systeme modular und flexibel gestalten lassen.










