import Head from 'next/head'
import {Inter} from '@next/font/google'
import styles from '../styles/Home.module.css'
import Button from "../components/Button";
import {useState} from "react";

const inter = Inter({subsets: ['latin']})

export default function Home() {
    let [isActive, setIsActive] = useState(false);
    let onClick = () => {setIsActive(!isActive)}
    return (
        <>
            <Head>
                <title>️DUNE</title>
                <meta name="description" content="DUNE-Sandbox Web UI"/>
                <meta name="viewport" content="width=device-width, initial-scale=1"/>
                <link rel="icon" href="/favicon.ico"/>
            </Head>
            <main className={styles.main}>
                <Button active={isActive} onClick={onClick} text={"Hello World"} subtext={"Lorem Ipsum"} src={"/button_test.png"}/>
            </main>
        </>
    )
}
