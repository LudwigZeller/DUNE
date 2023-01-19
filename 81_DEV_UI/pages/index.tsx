import Head from 'next/head'
import styles from '../styles/Home.module.css'
import Button from "../components/Button";
import {useState} from "react";

export default function Home() {
    enum FILTER {
        NORMAL = "NORMAL",
        BLOCKCRAFT = "BLOCKCRAFT",
        DIFFERENCE = "DIFFERENCE",
        STRIPE = "STRIPE",
    }

    let [activeFilter, setActiveFilter] = useState(FILTER.NORMAL);
    let onClick = (filter: FILTER) => {
        setActiveFilter(filter)
        fetch("http://localhost:8080", {method: "POST", body: "FILTER: " + filter, mode: "no-cors"})
            .then(res => console.log(res))
            .catch((err) => console.log(err))
    }
    return (
        <>
            <Head>
                <title>️DUNE</title>
                <meta name="description" content="DUNE-Sandbox Web UI"/>
                <meta name="viewport" content="width=device-width, initial-scale=1"/>
                <link rel="icon" href="/favicon.ico"/>
            </Head>
            <main className={styles.main}>
                <Button active={activeFilter == FILTER.NORMAL} onClick={() => onClick(FILTER.NORMAL)}
                        text={"Smooth"}
                        subtext={"Farbübergänge werden schön und abgerundet dargestellt"}
                        src={"/button_test.png"}/>
                <Button
                        active={activeFilter == FILTER.BLOCKCRAFT} onClick={() => onClick(FILTER.BLOCKCRAFT)}
                        text={"BlockCraft"}
                        subtext={"Höhenschichtlinien werden in einem Blockstil gezeichnet"}
                        src={"/button_test.png"}/>
                <Button active={activeFilter == FILTER.STRIPE} onClick={() => onClick(FILTER.STRIPE)}
                        text={"Stripe"}
                        subtext={"Der Smooth Filter, aber mit ducking stripes boiiiii"}
                        src={"/button_test.png"}/>
                <Button active={activeFilter == FILTER.DIFFERENCE} onClick={() => onClick(FILTER.DIFFERENCE)}
                        text={"Difference"}
                        subtext={"Das erste aufgenommene Bild wird als Vergleich verwendet. Unterschiede am Sand werden angezeigt."}
                        src={"/button_test.png"}/>
            </main>
        </>
    )
}
